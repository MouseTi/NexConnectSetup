#include "SetupManager.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>

#define NEXCONNECT_MANIFEST_URL "https://github.com/MouseTi/NexConnect/releases/latest/download/manifest.json"

SetupManager::SetupManager(QObject* parent)
    : QObject(parent)
    , m_manifestUrl(QUrl(QString::fromUtf8(NEXCONNECT_MANIFEST_URL)))
{
}

QString SetupManager::installPath() const
{
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    // AppDataLocation returns something like C:\Users\<user>\AppData\Roaming\NexConnectSetup
    // We want C:\Users\<user>\AppData\Roaming\NexConnect
    QDir appDataDir(appData);
    appDataDir.cdUp(); // go to Roaming
    return appDataDir.filePath("NexConnect");
}

QVariantMap SetupManager::status() const
{
    QVariantMap map;
    map["status"] = stateName();
    map["message"] = m_message;
    map["version"] = m_version;
    map["downloaded"] = m_downloaded;
    map["total"] = m_total;
    map["progress"] = m_total > 0 ? static_cast<int>((m_downloaded * 100) / m_total) : 0;
    return map;
}

QString SetupManager::stateName() const
{
    switch (m_state) {
    case State::Checking: return "Checking";
    case State::Downloading: return "Downloading";
    case State::Installing: return "Installing";
    case State::Complete: return "Complete";
    case State::Error: return "Error";
    case State::Idle:
    default: return "Idle";
    }
}

void SetupManager::setState(State state)
{
    m_state = state;
    emitStatus();
}

void SetupManager::setError(const QString& message)
{
    m_state = State::Error;
    m_message = message;
    emitStatus();
    emit installationFailed(message);
}

void SetupManager::emitStatus()
{
    emit statusChanged(status());
}

void SetupManager::startInstallation()
{
    if (m_state == State::Downloading || m_state == State::Installing) {
        return;
    }

    m_files.clear();
    m_currentFileIndex = 0;
    m_downloaded = 0;
    m_total = 0;

    if (!createInstallDirectory()) {
        setError("Failed to create installation directory");
        return;
    }

    checkManifest();
}

bool SetupManager::createInstallDirectory()
{
    const QString path = installPath();
    QDir dir;
    if (!dir.mkpath(path)) {
        return false;
    }
    return true;
}

void SetupManager::checkManifest()
{
    setState(State::Checking);
    m_message = "Checking for latest version...";
    emitStatus();

    QNetworkRequest req(m_manifestUrl);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    req.setHeader(QNetworkRequest::UserAgentHeader, "NexConnectSetup/1.0");
    req.setTransferTimeout(5000);

    QNetworkReply* reply = m_network.get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleManifestReply(reply);
        reply->deleteLater();
    });
}

void SetupManager::handleManifestReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        setError("Failed to download manifest: " + reply->errorString());
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    if (!doc.isObject()) {
        setError("Invalid manifest format");
        return;
    }

    const QJsonObject obj = doc.object();
    m_version = obj.value("version").toString();
    const QString url = obj.value("url").toString();
    m_binaryUrl = QUrl(url);
    m_binarySha256 = obj.value("sha256").toString().trimmed().toLower();

    if (m_version.isEmpty() || !m_binaryUrl.isValid()) {
        setError("Manifest missing required fields");
        return;
    }

    // Parse support files
    const QJsonValue filesVal = obj.value("files");
    if (filesVal.isArray()) {
        const QJsonArray arr = filesVal.toArray();
        for (const QJsonValue& v : arr) {
            if (!v.isObject()) continue;
            const QJsonObject fo = v.toObject();
            FileEntry e;
            e.path = fo.value("path").toString();
            e.url = QUrl(fo.value("url").toString());
            e.sha256 = fo.value("sha256").toString().trimmed().toLower();
            if (e.path.isEmpty() || !e.url.isValid()) continue;
            if (e.path.contains("..") || QDir::isAbsolutePath(e.path)) continue;
            m_files.append(e);
        }
    }

    m_message = "Found NexConnect v" + m_version;
    emitStatus();

    // Start downloading
    QTimer::singleShot(500, this, &SetupManager::beginDownload);
}

void SetupManager::beginDownload()
{
    setState(State::Downloading);
    m_message = "Downloading NexConnect...";
    m_currentFileIndex = 0;
    emitStatus();

    downloadNextFile();
}

void SetupManager::downloadNextFile()
{
    // Download support files first
    if (m_currentFileIndex < m_files.size()) {
        const FileEntry& e = m_files[m_currentFileIndex];
        const QString destPath = QDir(installPath()).filePath(e.path);
        
        // Create parent directory
        QFileInfo fi(destPath);
        QDir().mkpath(fi.absolutePath());

        QFile* file = new QFile(destPath);
        if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            file->deleteLater();
            setError("Failed to create file: " + e.path);
            return;
        }

        m_message = "Downloading " + e.path + "...";
        m_downloaded = 0;
        m_total = 0;
        emitStatus();

        QNetworkRequest req(e.url);
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        req.setHeader(QNetworkRequest::UserAgentHeader, "NexConnectSetup/1.0");

        QNetworkReply* reply = m_network.get(req);
        file->setParent(reply);

        connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
            file->write(reply->readAll());
        });
        connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 received, qint64 total) {
            m_downloaded = received;
            m_total = total;
            emitStatus();
        });
        connect(reply, &QNetworkReply::finished, this, [this, reply, file, destPath, e]() {
            file->write(reply->readAll());
            handleFileDownloadFinished(reply, file, destPath, e);
            reply->deleteLater();
        });
        return;
    }

    // Download main binary
    const QString binaryPath = QDir(installPath()).filePath("NexConnect.exe");
    QFile* file = new QFile(binaryPath);
    if (!file->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file->deleteLater();
        setError("Failed to create NexConnect.exe");
        return;
    }

    m_message = "Downloading NexConnect.exe...";
    m_downloaded = 0;
    m_total = 0;
    emitStatus();

    QNetworkRequest req(m_binaryUrl);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    req.setHeader(QNetworkRequest::UserAgentHeader, "NexConnectSetup/1.0");

    QNetworkReply* reply = m_network.get(req);
    file->setParent(reply);

    connect(reply, &QNetworkReply::readyRead, this, [reply, file]() {
        file->write(reply->readAll());
    });
    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 received, qint64 total) {
        m_downloaded = received;
        m_total = total;
        emitStatus();
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply, file, binaryPath]() {
        file->write(reply->readAll());
        handleBinaryDownloadFinished(reply, file, binaryPath);
        reply->deleteLater();
    });
}

void SetupManager::handleFileDownloadFinished(QNetworkReply* reply, QFile* file, const QString& path, const FileEntry& entry)
{
    file->flush();
    file->close();

    if (reply->error() != QNetworkReply::NoError) {
        QFile::remove(path);
        setError("Download failed: " + reply->errorString());
        return;
    }

    if (!entry.sha256.isEmpty() && !verifySha256(path, entry.sha256)) {
        QFile::remove(path);
        setError("SHA256 verification failed for: " + entry.path);
        return;
    }

    m_currentFileIndex++;
    downloadNextFile();
}

void SetupManager::handleBinaryDownloadFinished(QNetworkReply* reply, QFile* file, const QString& path)
{
    file->flush();
    file->close();

    if (reply->error() != QNetworkReply::NoError) {
        QFile::remove(path);
        setError("Binary download failed: " + reply->errorString());
        return;
    }

    if (!m_binarySha256.isEmpty() && !verifySha256(path, m_binarySha256)) {
        QFile::remove(path);
        setError("SHA256 verification failed for NexConnect.exe");
        return;
    }

    finishInstallation();
}

bool SetupManager::verifySha256(const QString& filePath, const QString& expected) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;
    QCryptographicHash hash(QCryptographicHash::Sha256);
    while (!file.atEnd()) {
        hash.addData(file.read(1024 * 1024));
    }
    return QString::fromLatin1(hash.result().toHex()).compare(expected.trimmed(), Qt::CaseInsensitive) == 0;
}

void SetupManager::finishInstallation()
{
    setState(State::Installing);
    m_message = "Creating shortcuts...";
    emitStatus();

    if (!createDesktopShortcut()) {
        // Non-fatal, continue anyway
    }

    setState(State::Complete);
    m_message = "Installation complete!";
    emitStatus();

    emit installationComplete();

    // Launch NexConnect after 1 second
    QTimer::singleShot(1000, this, &SetupManager::launchNexConnect);
}

bool SetupManager::createDesktopShortcut()
{
#ifdef Q_OS_WIN
    const QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    const QString shortcutPath = QDir(desktopPath).filePath("NexConnect.lnk");
    const QString targetPath = QDir(installPath()).filePath("NexConnect.exe");

    // Create PowerShell script to create shortcut
    QStringList psScript;
    psScript << "$WScriptShell = New-Object -ComObject WScript.Shell";
    psScript << QString("$Shortcut = $WScriptShell.CreateShortcut('%1')").arg(QDir::toNativeSeparators(shortcutPath));
    psScript << QString("$Shortcut.TargetPath = '%1'").arg(QDir::toNativeSeparators(targetPath));
    psScript << QString("$Shortcut.WorkingDirectory = '%1'").arg(QDir::toNativeSeparators(installPath()));
    psScript << "$Shortcut.Save()";

    const QString script = psScript.join("; ");
    
    QProcess proc;
    proc.start("powershell.exe", QStringList() << "-NoProfile" << "-Command" << script);
    proc.waitForFinished(5000);
    
    return proc.exitCode() == 0;
#else
    return false;
#endif
}

void SetupManager::launchNexConnect()
{
    const QString exePath = QDir(installPath()).filePath("NexConnect.exe");
    const QString workDir = installPath();
    
    bool started = QProcess::startDetached(exePath, QStringList(), workDir);
    if (started) {
        QTimer::singleShot(500, qApp, &QCoreApplication::quit);
    }
}
