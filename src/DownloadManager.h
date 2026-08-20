#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QString>

class QFile;
class QNetworkReply;

class DownloadManager : public QObject {
    Q_OBJECT

public:
    explicit DownloadManager(QObject* parent = nullptr);

    void startDownload();
    QString installPath() const;

Q_SIGNALS:
    void progressChanged(int percent, const QString& status);
    void downloadComplete(bool success, const QString& message);
    void checkFinished();

private:
    enum class State { Idle, CheckingUpdate, Downloading, Installing, Complete, Error };

    void checkManifest();
    void handleManifestReply(QNetworkReply* reply);
    void downloadFile(const QString& fileName, const QUrl& url, const QString& sha256);
    void handleFileDownloadFinished(QNetworkReply* reply, QFile* file, 
                                   const QString& tempPath, const QString& fileName, 
                                   const QString& expectedHash);
    void downloadNextFile();
    bool verifySha256(const QString& filePath, const QString& expected) const;
    bool createDesktopShortcut();
    void launchNexConnect();

    QNetworkAccessManager m_network;
    State m_state = State::Idle;
    QString m_installDir;
    
    struct FileInfo {
        QString fileName;
        QUrl url;
        QString sha256;
    };
    
    QList<FileInfo> m_filesToDownload;
    int m_filesDownloaded = 0;
    qint64 m_currentDownloaded = 0;
    qint64 m_currentTotal = 0;
};
