#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QVariantMap>

class QFile;
class QNetworkReply;

/**
 * SetupManager - Downloads and installs NexConnect from GitHub releases.
 *
 * Installation flow:
 *   1. Create %APPDATA%\NexConnect directory
 *   2. Download manifest.json from GitHub releases/latest
 *   3. Download NexConnect.exe and support files (nexus_runtime.dll, etc.)
 *   4. Verify SHA256 checksums
 *   5. Create desktop shortcut
 *   6. Launch NexConnect.exe
 */
class SetupManager : public QObject {
    Q_OBJECT

public:
    explicit SetupManager(QObject* parent = nullptr);

    QVariantMap status() const;
    void startInstallation();

Q_SIGNALS:
    void statusChanged(const QVariantMap& status);
    void installationComplete();
    void installationFailed(const QString& error);

private:
    enum class State { Idle, Checking, Downloading, Installing, Complete, Error };

    struct FileEntry {
        QString path;
        QUrl url;
        QString sha256;
    };

    void setState(State state);
    void setError(const QString& message);
    void emitStatus();
    QString stateName() const;
    QString installPath() const;
    bool createInstallDirectory();
    bool createDesktopShortcut();
    void launchNexConnect();
    
    void checkManifest();
    void handleManifestReply(QNetworkReply* reply);
    void beginDownload();
    void downloadNextFile();
    void handleFileDownloadFinished(QNetworkReply* reply, QFile* file, const QString& path, const FileEntry& entry);
    void handleBinaryDownloadFinished(QNetworkReply* reply, QFile* file, const QString& path);
    bool verifySha256(const QString& filePath, const QString& expected) const;
    void finishInstallation();

    QNetworkAccessManager m_network;
    QUrl m_manifestUrl;
    QUrl m_binaryUrl;
    QString m_binarySha256;
    QString m_version;
    QString m_message;
    State m_state = State::Idle;
    qint64 m_downloaded = 0;
    qint64 m_total = 0;

    QList<FileEntry> m_files;
    int m_currentFileIndex = 0;
};
