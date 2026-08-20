#include "DownloadManager.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

DownloadManager::DownloadManager(QObject* parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
    , m_reply(nullptr)
    , m_file(nullptr)
    , m_bytesReceived(0)
    , m_totalBytes(0)
{
}

DownloadManager::~DownloadManager()
{
    cancel();
}

void DownloadManager::startDownload(const QString& url, const QString& destPath)
{
    cancel();

    m_currentUrl = url;
    m_bytesReceived = 0;
    m_totalBytes = 0;

    QFileInfo fileInfo(destPath);
    QDir().mkpath(fileInfo.absolutePath());

    m_file = new QFile(destPath, this);
    if (!m_file->open(QIODevice::WriteOnly)) {
        emit downloadComplete(false, "Failed to create file: " + destPath);
        delete m_file;
        m_file = nullptr;
        return;
    }

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, 
                        QNetworkRequest::NoLessSafeRedirectPolicy);
    request.setRawHeader("User-Agent", "NexConnect-Setup/1.0");

    m_reply = m_network->get(request);
    
    connect(m_reply, &QNetworkReply::downloadProgress, 
            this, &DownloadManager::onDownloadProgress);
    connect(m_reply, &QNetworkReply::finished, 
            this, &DownloadManager::onFinished);
    connect(m_reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &DownloadManager::onError);
    connect(m_reply, &QNetworkReply::readyRead, this, [this]() {
        if (m_file && m_reply) {
            m_file->write(m_reply->readAll());
        }
    });

    emit progressChanged(0, "Connecting...");
}

void DownloadManager::cancel()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    if (m_file) {
        m_file->close();
        m_file->deleteLater();
        m_file = nullptr;
    }
}

int DownloadManager::progress() const
{
    if (m_totalBytes <= 0) return 0;
    return static_cast<int>((m_bytesReceived * 100) / m_totalBytes);
}

void DownloadManager::onDownloadProgress(qint64 received, qint64 total)
{
    m_bytesReceived = received;
    m_totalBytes = total;

    int percent = progress();
    QString status = QString("Downloading... %1 MB / %2 MB")
        .arg(received / 1024.0 / 1024.0, 0, 'f', 2)
        .arg(total / 1024.0 / 1024.0, 0, 'f', 2);

    emit progressChanged(percent, status);
    emit downloadProgress(received, total);
}

void DownloadManager::onFinished()
{
    if (!m_reply) return;

    if (m_file) {
        if (m_reply->error() == QNetworkReply::NoError) {
            m_file->write(m_reply->readAll());
        }
        m_file->close();
    }

    QNetworkReply::NetworkError error = m_reply->error();
    QString errorString = m_reply->errorString();

    m_reply->deleteLater();
    m_reply = nullptr;

    if (m_file) {
        m_file->deleteLater();
        m_file = nullptr;
    }

    if (error == QNetworkReply::NoError) {
        emit progressChanged(100, "Download complete");
        emit downloadComplete(true, "Download successful");
    } else {
        emit downloadComplete(false, "Download failed: " + errorString);
    }
}

void DownloadManager::onError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    // Error handling is done in onFinished
}
