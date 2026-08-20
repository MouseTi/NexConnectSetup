#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QFile>

class DownloadManager : public QObject {
    Q_OBJECT

public:
    explicit DownloadManager(QObject* parent = nullptr);
    ~DownloadManager();

    void startDownload(const QString& url, const QString& destPath);
    void cancel();

    qint64 bytesReceived() const { return m_bytesReceived; }
    qint64 totalBytes() const { return m_totalBytes; }
    int progress() const;

signals:
    void progressChanged(int percent, const QString& status);
    void downloadComplete(bool success, const QString& message);
    void downloadProgress(qint64 received, qint64 total);

private slots:
    void onDownloadProgress(qint64 received, qint64 total);
    void onFinished();
    void onError(QNetworkReply::NetworkError code);

private:
    QNetworkAccessManager* m_network;
    QNetworkReply* m_reply;
    QFile* m_file;
    qint64 m_bytesReceived;
    qint64 m_totalBytes;
    QString m_currentUrl;
};
