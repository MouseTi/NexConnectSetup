#include "SetupWindow.h"
#include "DownloadManager.h"
#include "SetupManager.h"
#include <QPainter>
#include <QPainterPath>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
#include <QDebug>

SetupWindow::SetupWindow(QWidget* parent)
    : QWidget(parent)
    , m_downloadManager(nullptr)
    , m_progress(0)
    , m_status("Initializing...")
    , m_isComplete(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setFixedSize(500, 300);

    QRect screen = QGuiApplication::primaryScreen()->geometry();
    move((screen.width() - width()) / 2, (screen.height() - height()) / 2);

    // Create setup manager
    SetupManager* setupManager = new SetupManager(this);
    
    connect(setupManager, &SetupManager::progressChanged,
            this, &SetupWindow::onProgressChanged);
    connect(setupManager, &SetupManager::setupComplete,
            this, &SetupWindow::onDownloadComplete);

    // Start setup after a short delay
    QTimer::singleShot(500, this, [setupManager]() {
        setupManager->startSetup();
    });

    // Update UI periodically
    QTimer* updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, [this]() {
        update();
    });
    updateTimer->start(16); // ~60fps
}

void SetupWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect r = rect();
    int radius = 12;
    
    QPainterPath path;
    path.addRoundedRect(QRectF(r), radius, radius);
    p.setClipPath(path);

    // Dark background
    p.fillRect(r, QColor("#0B0E14"));

    // Border
    QColor borderCol = QColor(255, 255, 255, 30);
    p.setPen(QPen(borderCol, 1));
    p.drawRoundedRect(r.adjusted(0, 0, -1, -1), radius, radius);

    // Title
    p.setPen(QColor(248, 250, 252));
    QFont titleFont = p.font();
    titleFont.setPixelSize(20);
    titleFont.setBold(true);
    titleFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.2);
    p.setFont(titleFont);
    p.drawText(r.adjusted(30, 30, 0, 0), Qt::AlignTop | Qt::AlignLeft, "NexConnect Setup");

    // Status text
    QFont statusFont = p.font();
    statusFont.setPixelSize(13);
    statusFont.setBold(false);
    p.setFont(statusFont);
    p.setPen(QColor(156, 163, 175));
    
    QRect statusRect = r.adjusted(30, 80, -30, 0);
    p.drawText(statusRect, Qt::AlignTop | Qt::AlignLeft, m_status);

    // Progress bar background
    int barY = r.height() - 100;
    int barWidth = r.width() - 60;
    QRect barBg(30, barY, barWidth, 8);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(31, 41, 55));
    p.drawRoundedRect(barBg, 4, 4);

    // Progress bar fill
    if (m_progress > 0) {
        int fillWidth = (barWidth * m_progress) / 100;
        QRect barFill(30, barY, fillWidth, 8);
        
        // Gradient fill
        QLinearGradient gradient(barFill.topLeft(), barFill.topRight());
        gradient.setColorAt(0, QColor("#38BDF8"));
        gradient.setColorAt(1, QColor("#22D3EE"));
        p.setBrush(gradient);
        p.drawRoundedRect(barFill, 4, 4);
    }

    // Progress percentage
    p.setPen(QColor(248, 250, 252));
    QFont percentFont = p.font();
    percentFont.setPixelSize(24);
    percentFont.setBold(true);
    p.setFont(percentFont);
    QString percentText = QString("%1%").arg(m_progress);
    p.drawText(r.adjusted(0, 0, 0, -60), Qt::AlignCenter, percentText);

    // Complete message
    if (m_isComplete) {
        p.setPen(QColor(34, 211, 238));
        QFont msgFont = p.font();
        msgFont.setPixelSize(14);
        p.setFont(msgFont);
        p.drawText(r.adjusted(0, 0, 0, -20), Qt::AlignCenter | Qt::AlignBottom, m_finalMessage);
    }
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
    m_finalMessage = success ? "✓ " + message : "✗ " + message;
    m_progress = success ? 100 : m_progress;
    update();

    if (success) {
        // Close window after 2 seconds
        QTimer::singleShot(2000, this, &QWidget::close);
    }
}
