#ifndef MANIFESTPARSER_H
#define MANIFESTPARSER_H

#include <string>

struct ManifestInfo {
    std::wstring version;
    std::wstring downloadUrl;
    std::wstring sha256;
    bool isValid;
    
    ManifestInfo() : isValid(false) {}
};

class ManifestParser {
public:
    static ManifestInfo ParseFromUrl(const std::wstring& manifestUrl);
    static ManifestInfo ParseFromJson(const std::string& jsonContent);
    
private:
    static std::string DownloadManifest(const std::wstring& url);
    static std::string GetJsonValue(const std::string& json, const std::string& key);
};

#endif // MANIFESTPARSER_H
