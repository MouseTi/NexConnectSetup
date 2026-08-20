#include "SetupManager.h"
#include "DownloadManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QProcess>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <shobjidl.h>

SetupManager::SetupManager(QObject* parent)
    : QObject(parent)
    , m_downloader(new DownloadManager(this))
    , m_currentFileIndex(0)
    , m_totalSize(0)
    , m_downloadedSize(0)
    , m_currentStage(Stage::Initializing)
{
    // Get AppData/Local path
    QString appData = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    m_installPath = appData.left(appData.lastIndexOf('/')) + "/NexConnect";

    connect(m_downloader, &DownloadManager::progressChanged,
            this, &SetupManager::progressChanged);
    connect(m_downloader, &DownloadManager::downloadComplete,
            this, &SetupManager::onFileDownloaded);
}

QString SetupManager::getInstallPath() const
{
    return m_installPath;
}

void SetupManager::startSetup()
{
    m_currentStage = Stage::CreatingDirectories;
    emit stageChanged("Creating directories");
    createDirectories();
}

void SetupManager::createDirectories()
{
    emit progressChanged(5, "Creating installation directory...");
    
    QDir dir;
    if (!dir.exists(m_installPath)) {
        if (!dir.mkpath(m_installPath)) {
            emit setupComplete(false, "Failed to create installation directory");
            return;
        }
    }

    m_currentStage = Stage::DownloadingManifest;
    emit stageChanged("Downloading manifest");
    downloadManifest();
}

void SetupManager::downloadManifest()
{
    emit progressChanged(10, "Fetching file list...");
    
    // GitHub release manifest URL
    // Replace with your actual GitHub release URL
    QString manifestUrl = "https://raw.githubusercontent.com/MouseTi/NexConnectSetup/main/manifest.json";
    
    // For now, use hardcoded file list
    // TODO: Download and parse manifest from GitHub
    
    m_filesToDownload.clear();
    
    // Main executable
    m_filesToDownload.append({
        "https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/NexConnect.exe",
        "NexConnect.exe",
        5 * 1024 * 1024  // 5MB estimate
    });
    
    // DLL files
    m_filesToDownload.append({
        "https://github.com/MouseTi/NexConnectSetup/releases/download/v1.0.0/nexus_runtime.dll",
        "nexus_runtime.dll",
        2 * 1024 * 1024  // 2MB estimate
    });

    // Calculate total size
    m_totalSize = 0;
    for (const auto& file : m_filesToDownload) {
        m_totalSize += file.size;
    }

    m_currentStage = Stage::DownloadingFiles;
    emit stageChanged("Downloading files");
    downloadFiles();
}

void SetupManager::downloadFiles()
{
    m_currentFileIndex = 0;
    m_downloadedSize = 0;
    downloadNextFile();
}

void SetupManager::downloadNextFile()
{
    if (m_currentFileIndex >= m_filesToDownload.size()) {
        // All files downloaded
        m_currentStage = Stage::CreatingShortcut;
        emit stageChanged("Creating shortcut");
        createDesktopShortcut();
        return;
    }

    const FileToDownload& file = m_filesToDownload[m_currentFileIndex];
    QString destPath = m_installPath + "/" + file.filename;
    
    int overallProgress = 20 + (m_currentFileIndex * 60 / m_filesToDownload.size());
    emit progressChanged(overallProgress, 
        QString("Downloading %1/%2: %3")
            .arg(m_currentFileIndex + 1)
            .arg(m_filesToDownload.size())
            .arg(file.filename));

    m_downloader->startDownload(file.url, destPath);
}

void SetupManager::onFileDownloaded(bool success, const QString& message)
{
    if (!success) {
        emit setupComplete(false, "Download failed: " + message);
        return;
    }

    m_currentFileIndex++;
    downloadNextFile();
}

void SetupManager::createDesktopShortcut()
{
    emit progressChanged(85, "Creating desktop shortcut...");

    // Get desktop path
    wchar_t desktopPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopPath))) {
        QString shortcutPath = QString::fromWCharArray(desktopPath) + "/NexConnect.lnk";
        QString targetPath = m_installPath + "/NexConnect.exe";

        // Create shortcut using COM
        CoInitialize(NULL);
        
        IShellLinkW* pShellLink = NULL;
        HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                       IID_IShellLinkW, (LPVOID*)&pShellLink);
        
        if (SUCCEEDED(hres)) {
            pShellLink->SetPath(targetPath.toStdWString().c_str());
            pShellLink->SetWorkingDirectory(m_installPath.toStdWString().c_str());
            pShellLink->SetDescription(L"NexConnect Launcher");

            IPersistFile* pPersistFile = NULL;
            hres = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
            
            if (SUCCEEDED(hres)) {
                pPersistFile->Save(shortcutPath.toStdWString().c_str(), TRUE);
                pPersistFile->Release();
            }
            
            pShellLink->Release();
        }
        
        CoUninitialize();
    }

    m_currentStage = Stage::Launching;
    emit stageChanged("Launching application");
    launchApplication();
}

void SetupManager::launchApplication()
{
    emit progressChanged(95, "Launching NexConnect...");

    QString exePath = m_installPath + "/NexConnect.exe";
    
    if (QFile::exists(exePath)) {
        // Launch the application
        QProcess::startDetached(exePath, QStringList(), m_installPath);
        
        // Wait a moment then finish
        QTimer::singleShot(1000, this, &SetupManager::finishSetup);
    } else {
        emit setupComplete(false, "Installation complete but cannot find NexConnect.exe");
    }
}

void SetupManager::finishSetup()
{
    m_currentStage = Stage::Complete;
    emit progressChanged(100, "Setup complete!");
    emit setupComplete(true, "NexConnect has been installed successfully");
}
