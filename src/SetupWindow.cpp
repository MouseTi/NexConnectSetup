#include "SetupWindow.h"
#include "DownloadManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>

SetupWindow::SetupWindow(QWidget* parent)
    : QWidget(parent)
    , m_downloadManager(new DownloadManager(this))
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(480, 320);
    
    QRect screen = QGuiApplication::primaryScreen()->geometry();
    move((screen.width() - width()) / 2, (screen.height() - height()) / 2);
    
    connect(m_downloadManager, &DownloadManager::progressChanged,
            this, &SetupWindow::onProgressChanged);
    connect(m_downloadManager, &DownloadManager::downloadComplete,
            this, &SetupWindow::onDownloadComplete);
    
    // Start download after 500ms
    QTimer::singleShot(500, m_downloadManager, &DownloadManager::startDownload);
}

void SetupWindow::onProgressChanged(int percent, const QString& status)
{
    m_progress = percent;
    m_status = status;
    update();
}

void SetupWindow::onDownloadComplete(bool success, const QString& message)
{
    m_isComplete = true;
    m_finalMessage = message;
    m_progress = 100;
    update();
}

void SetupWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    
    QRect r = rect();
    int radius = 12;
    
    QPainterPath path;
    path.addRoundedRect(QRectF(r), radius, radius);
    p.setClipPath(path);
    
    // Background - deep slate
    QLinearGradient bgGrad(0, 0, 0, r.height());
    bgGrad.setColorAt(0, QColor("#0F172A"));
    bgGrad.setColorAt(1, QColor("#0B0E14"));
    p.fillRect(r, bgGrad);
    
    // Hairline border
    p.setPen(QPen(QColor(255, 255, 255, 20), 1));
    p.drawRoundedRect(r.adjusted(0, 0, -1, -1), radius, radius);
    
    // Logo/Title area
    QFont titleFont = p.font();
    titleFont.setPixelSize(28);
    titleFont.setBold(true);
    titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.5);
    p.setFont(titleFont);
    p.setPen(QColor("#F8FAFC"));
    p.drawText(r.adjusted(32, 40, -32, 0), Qt::AlignTop | Qt::AlignLeft, "NexConnect");
    
    QFont subFont = p.font();
    subFont.setPixelSize(13);
    subFont.setBold(false);
    subFont.setLetterSpacing(QFont::AbsoluteSpacing, 0.3);
    p.setFont(subFont);
    p.setPen(QColor(248, 250, 252, 160));
    p.drawText(r.adjusted(32, 76, -32, 0), Qt::AlignTop | Qt::AlignLeft, "Setup Installer");
    
    // Progress bar background
    int barY = r.height() - 120;
    int barHeight = 8;
    QRect barBg(32, barY, r.width() - 64, barHeight);
    
    QPainterPath barBgPath;
    barBgPath.addRoundedRect(barBg, 4, 4);
    p.fillPath(barBgPath, QColor(255, 255, 255, 25));
    
    // Progress bar fill (cyan accent)
    if (m_progress > 0) {
        int fillWidth = (barBg.width() * m_progress) / 100;
        QRect barFill(barBg.x(), barBg.y(), fillWidth, barHeight);
        
        QLinearGradient fillGrad(barFill.left(), 0, barFill.right(), 0);
        fillGrad.setColorAt(0, QColor("#22D3EE"));
        fillGrad.setColorAt(1, QColor("#38BDF8"));
        
        QPainterPath barFillPath;
        barFillPath.addRoundedRect(barFill, 4, 4);
        p.fillPath(barFillPath, fillGrad);
    }
    
    // Status text
    QFont statusFont = p.font();
    statusFont.setPixelSize(12);
    statusFont.setBold(false);
    p.setFont(statusFont);
    p.setPen(QColor(248, 250, 252, 200));
    p.drawText(r.adjusted(32, barY + 24, -32, 0), Qt::AlignTop | Qt::AlignLeft, m_status);
    
    // Percentage
    QFont percentFont = p.font();
    percentFont.setPixelSize(14);
    percentFont.setBold(true);
    percentFont.setFamily("Consolas");
    p.setFont(percentFont);
    p.setPen(QColor("#22D3EE"));
    QString percentText = QString("%1%").arg(m_progress);
    p.drawText(r.adjusted(32, barY + 44, -32, 0), Qt::AlignTop | Qt::AlignLeft, percentText);
    
    // Install path info
    if (m_progress > 0 && m_progress < 100) {
        QFont pathFont = p.font();
        pathFont.setPixelSize(10);
        pathFont.setBold(false);
        p.setFont(pathFont);
        p.setPen(QColor(248, 250, 252, 100));
        QString installPath = QString("Installing to: %1").arg(m_downloadManager->installPath());
        p.drawText(r.adjusted(32, r.height() - 32, -32, -16), 
                  Qt::AlignBottom | Qt::AlignLeft, installPath);
    }
    
    // Complete message with orange accent
    if (m_isComplete) {
        QFont completeFont = p.font();
        completeFont.setPixelSize(13);
        completeFont.setBold(true);
        p.setFont(completeFont);
        p.setPen(QColor("#F97316"));
        p.drawText(r.adjusted(32, barY + 64, -32, 0), 
                  Qt::AlignTop | Qt::AlignLeft, m_finalMessage);
    }
}
