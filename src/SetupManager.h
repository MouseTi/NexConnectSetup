#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <QVariantMap>

class DownloadManager;

class SetupManager : public QObject {
    Q_OBJECT

public:
    explicit SetupManager(QObject* parent = nullptr);
    ~SetupManager() = default;

    void startSetup();
    QString getInstallPath() const;
    
signals:
    void progressChanged(int percent, const QString& status);
    void setupComplete(bool success, const QString& message);
    void stageChanged(const QString& stage);

private:
    struct FileToDownload {
        QString url;
        QString filename;
        qint64 size;
    };

    void createDirectories();
    void downloadManifest();
    void downloadFiles();
    void downloadNextFile();
    void onFileDownloaded(bool success, const QString& message);
    void createDesktopShortcut();
    void launchApplication();
    void finishSetup();

    DownloadManager* m_downloader;
    QString m_installPath;
    QList<FileToDownload> m_filesToDownload;
    int m_currentFileIndex;
    qint64 m_totalSize;
    qint64 m_downloadedSize;
    
    enum class Stage {
        Initializing,
        CreatingDirectories,
        DownloadingManifest,
        DownloadingFiles,
        CreatingShortcut,
        Launching,
        Complete
    };
    Stage m_currentStage;
};
