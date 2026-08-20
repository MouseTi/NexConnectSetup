#include "ManifestParser.h"
#include "Utils.h"
#include <wininet.h>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "wininet.lib")

std::string ManifestParser::DownloadManifest(const std::wstring& url) {
    std::string result;
    
    HINTERNET hInternet = InternetOpenW(
        L"NexConnect Setup/1.0",
        INTERNET_OPEN_TYPE_DIRECT,
        NULL,
        NULL,
        0
    );
    
    if (!hInternet) return result;
    
    HINTERNET hUrl = InternetOpenUrlW(
        hInternet,
        url.c_str(),
        NULL,
        0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE,
        0
    );
    
    if (hUrl) {
        const DWORD BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        DWORD bytesRead = 0;
        
        while (InternetReadFile(hUrl, buffer, BUFFER_SIZE - 1, &bytesRead) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            result += buffer;
        }
        
        InternetCloseHandle(hUrl);
    }
    
    InternetCloseHandle(hInternet);
    return result;
}

std::string ManifestParser::GetJsonValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    
    if (pos == std::string::npos) return "";
    
    // Find the colon after the key
    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";
    
    // Skip whitespace and find opening quote
    pos = json.find('"', pos);
    if (pos == std::string::npos) return "";
    
    pos++; // Move past the opening quote
    
    // Find closing quote
    size_t endPos = json.find('"', pos);
    if (endPos == std::string::npos) return "";
    
    return json.substr(pos, endPos - pos);
}

ManifestInfo ManifestParser::ParseFromUrl(const std::wstring& manifestUrl) {
    std::string jsonContent = DownloadManifest(manifestUrl);
    return ParseFromJson(jsonContent);
}

ManifestInfo ManifestParser::ParseFromJson(const std::string& jsonContent) {
    ManifestInfo info;
    
    if (jsonContent.empty()) {
        return info;
    }
    
    // Parse JSON manually (simple key-value extraction)
    std::string version = GetJsonValue(jsonContent, "version");
    std::string url = GetJsonValue(jsonContent, "url");
    std::string sha256 = GetJsonValue(jsonContent, "sha256");
    
    if (!version.empty() && !url.empty()) {
        info.version = Utils::StringToWString(version);
        info.downloadUrl = Utils::StringToWString(url);
        info.sha256 = Utils::StringToWString(sha256);
        info.isValid = true;
    }
    
    return info;
}
