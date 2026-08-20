#include "DownloadManager.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QCoreApplication>
#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>

DownloadManager::DownloadManager(QObject* parent)
    : QObject(parent)
{
    // AppData\Roaming\NexConnect
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_installDir = appData.replace("NexConnectSetup", "NexConnect");
}

QString DownloadManager::installPath() const
{
    return m_installDir;
}

void DownloadManager::startDownload()
{
    m_state = State::CheckingUpdate;
    emit progressChanged(0, "Checking for NexConnect...");
    
    // Create install directory
    QDir().mkpath(m_installDir);
    
    checkManifest();
}

void DownloadManager::checkManifest()
{
    QUrl manifestUrl("https://github.com/MouseTi/NexConnect/releases/latest/download/manifest.json");
    
    QNetworkRequest req(manifestUrl);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, 
                     QNetworkRequest::NoLessSafeRedirectPolicy);
    req.setHeader(QNetworkRequest::UserAgentHeader, "NexConnectSetup/1.0");
    req.setTransferTimeout(10000);
    
    QNetworkReply* reply = m_network.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleManifestReply(reply);
        reply->deleteLater();
    });
}

void DownloadManager::handleManifestReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit downloadComplete(false, "Failed to check update: " + reply->errorString());
        return;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (!doc.isObject()) {
        emit downloadComplete(false, "Invalid manifest format");
        return;
    }
    
    QJsonObject obj = doc.object();
    QString version = obj.value("version").toString();
    QString binaryUrl = obj.value("url").toString();
    QString binarySha256 = obj.value("sha256").toString();
    
    if (binaryUrl.isEmpty()) {
        emit downloadComplete(false, "No download URL in manifest");
        return;
    }
    
    // Add main binary
    FileInfo mainFile;
    mainFile.fileName = "NexConnect.exe";
    mainFile.url = QUrl(binaryUrl);
    mainFile.sha256 = binarySha256;
    m_filesToDownload.append(mainFile);
    
    // Add support files (DLLs, etc.)
    QJsonArray filesArray = obj.value("files").toArray();
    for (const QJsonValue& val : filesArray) {
        if (!val.isObject()) continue;
        QJsonObject fileObj = val.toObject();
        
        FileInfo info;
        info.fileName = fileObj.value("path").toString();
        info.url = QUrl(fileObj.value("url").toString());
        info.sha256 = fileObj.value("sha256").toString();
        
        if (!info.fileName.isEmpty() && info.url.isValid()) {
            m_filesToDownload.append(info);
        }
    }
    
    emit progressChanged(5, QString("Found NexConnect v%1").arg(version));
    emit checkFinished();
    
    // Start downloading files
    m_filesDownloaded = 0;
    downloadNextFile();
}

void DownloadManager::downloadNextFile()
{
    if (m_filesToDownload.isEmpty()) {
        // All files downloaded
        m_state = State::Installing;
        emit progressChanged(95, "Creating desktop shortcut...");
        
        if (createDesktopShortcut()) {
            emit progressChanged(100, "Installation complete!");
            m_state = State::Complete;
            
            // Launch NexConnect after 1 second
            QTimer::singleShot(1000, this, &DownloadManager::launchNexConnect);
        } else {
            emit downloadComplete(true, "Installed but failed to create shortcut");
        }
        return;
    }
    
    FileInfo info = m_filesToDownload.takeFirst();
    m_state = State::Downloading;
    
    QString filePath = QDir(m_installDir).filePath(info.fileName);
    
    // Create subdirectories if needed
    QFileInfo fi(filePath);
    QDir().mkpath(fi.absolutePath());
    
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit downloadComplete(false, "Failed to create file: " + info.fileName);
        file->deleteLater();
        return;
    }
    
    int totalFiles = m_filesDownloaded + m_filesToDownload.size() + 1;
    emit progressChanged(10 + (m_filesDownloaded * 80 / totalFiles), 
                        QString("Downloading %1...").arg(info.fileName));
    
    QNetworkRequest req(info.url);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                     QNetworkRequest::NoLessSafeRedirectPolicy);
    req.setHeader(QNetworkRequest::UserAgentHeader, "NexConnectSetup/1.0");
    
    QNetworkReply* reply = m_network.get(req);
    file->setParent(reply);
    
    connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
        file->write(reply->readAll());
    });
    
    connect(reply, &QNetworkReply::downloadProgress, this, 
            [this, info, totalFiles](qint64 received, qint64 total) {
        m_currentDownloaded = received;
        m_currentTotal = total;
        int baseProgress = 10 + (m_filesDownloaded * 80 / totalFiles);
        int fileProgress = (total > 0) ? (int)(received * 80 / totalFiles / total) : 0;
        emit progressChanged(baseProgress + fileProgress, 
                           QString("Downloading %1... %2%").arg(info.fileName)
                           .arg(total > 0 ? (received * 100 / total) : 0));
    });
    
    connect(reply, &QNetworkReply::finished, this, 
            [this, reply, file, filePath, info]() {
        file->write(reply->readAll());
        handleFileDownloadFinished(reply, file, filePath, info.fileName, info.sha256);
        reply->deleteLater();
    });
}

void DownloadManager::handleFileDownloadFinished(QNetworkReply* reply, QFile* file,
                                                 const QString& tempPath, 
                                                 const QString& fileName,
                                                 const QString& expectedHash)
{
    file->flush();
    file->close();
    
    if (reply->error() != QNetworkReply::NoError) {
        QFile::remove(tempPath);
        emit downloadComplete(false, QString("Failed to download %1: %2")
                             .arg(fileName, reply->errorString()));
        return;
    }
    
    // Verify SHA256 if provided
    if (!expectedHash.isEmpty() && !verifySha256(tempPath, expectedHash)) {
        QFile::remove(tempPath);
        emit downloadComplete(false, QString("SHA256 verification failed for %1").arg(fileName));
        return;
    }
    
    m_filesDownloaded++;
    downloadNextFile();
}

bool DownloadManager::verifySha256(const QString& filePath, const QString& expected) const
{
    if (expected.isEmpty()) return true;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;
    
    QCryptographicHash hash(QCryptographicHash::Sha256);
    while (!file.atEnd()) {
        hash.addData(file.read(1024 * 1024));
    }
    
    QString actual = QString::fromLatin1(hash.result().toHex());
    return actual.compare(expected.trimmed(), Qt::CaseInsensitive) == 0;
}

bool DownloadManager::createDesktopShortcut()
{
    CoInitialize(NULL);
    
    WCHAR desktopPath[MAX_PATH];
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopPath))) {
        CoUninitialize();
        return false;
    }
    
    QString shortcutPath = QString::fromWCharArray(desktopPath) + "\\NexConnect.lnk";
    QString targetPath = QDir(m_installDir).filePath("NexConnect.exe");
    
    IShellLinkW* pShellLink = NULL;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IShellLinkW, (void**)&pShellLink);
    
    if (SUCCEEDED(hr)) {
        pShellLink->SetPath(targetPath.toStdWString().c_str());
        pShellLink->SetWorkingDirectory(m_installDir.toStdWString().c_str());
        pShellLink->SetDescription(L"NexConnect Anticheat Launcher");
        
        IPersistFile* pPersistFile = NULL;
        hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
        
        if (SUCCEEDED(hr)) {
            hr = pPersistFile->Save(shortcutPath.toStdWString().c_str(), TRUE);
            pPersistFile->Release();
        }
        
        pShellLink->Release();
    }
    
    CoUninitialize();
    return SUCCEEDED(hr);
}

void DownloadManager::launchNexConnect()
{
    QString exePath = QDir(m_installDir).filePath("NexConnect.exe");
    
    if (QFile::exists(exePath)) {
        QProcess::startDetached(exePath, QStringList(), m_installDir);
        emit downloadComplete(true, "NexConnect launched successfully!");
        
        // Close setup after 2 seconds
        QTimer::singleShot(2000, qApp, []() {
            QCoreApplication::quit();
        });
    } else {
        emit downloadComplete(true, "Installed but NexConnect.exe not found");
    }
}
