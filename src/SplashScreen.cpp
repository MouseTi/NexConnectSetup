#include "SplashScreen.h"
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QGuiApplication>
#include <QDebug>
#include <QFile>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <QTimer>

SplashScreen::SplashScreen(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setFixedSize(370, 250);

    QRect screen = QGuiApplication::primaryScreen()->geometry();
    move((screen.width() - width()) / 2, (screen.height() - height()) / 2);

    setStyleSheet("background:transparent;");
}

void SplashScreen::fadeOutAndClose()
{
    m_animStopped = true; // stop the 12ms repaint loop before fade starts
    QPropertyAnimation* anim = new QPropertyAnimation(this, "windowOpacity");
    anim->setDuration(180);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
    connect(anim, &QPropertyAnimation::finished, this, [this]() {
        close();
        emit done();
    });
}

void SplashScreen::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRect r = rect();
    int radius = 0;
    QPainterPath path;
    path.addRoundedRect(QRectF(r), radius, radius);
    p.setClipPath(path);

    p.fillRect(r, QColor("#0D0D0D"));

    QColor borderCol = QColor(255, 255, 255, 18);
    p.setPen(QPen(borderCol, 1));
    p.drawRoundedRect(r.adjusted(0, 0, -1, -1), radius, radius);

    drawLoading(p, r);

    p.setPen(QColor(255, 255, 255, 180));
    QFont f = p.font();
    f.setPixelSize(13);
    f.setBold(true);
    f.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
    p.setFont(f);
    p.drawText(r.adjusted(16, 12, 0, 0), Qt::AlignTop | Qt::AlignLeft, "Nex");
}

void SplashScreen::drawLoading(QPainter& p, const QRect& rect)
{
    int cx = rect.width() / 2;
    int cy = rect.height() / 2;

    int R = 48;
    QRectF ring(cx - R, cy - R, R * 2, R * 2);

    QPen bgPen(QColor(255, 255, 255, 60), 7);
    bgPen.setCapStyle(Qt::RoundCap);
    p.setPen(bgPen);
    p.drawArc(ring, 0, 360 * 16);

    static double phase = 0.0;
    QPen spinPen(QColor(255, 255, 255), 7);
    spinPen.setCapStyle(Qt::RoundCap);
    p.setPen(spinPen);

    double angle = phase * 360.0;
    int span = -90 * 16;
    p.drawArc(ring, qRound(angle * 16), span);

    phase += 0.012;
    if (phase >= 1.0) phase = 0.0;

    QPixmap logo("F:/Nexconnect/src/png/nex-logo.png");
    if (!logo.isNull()) {
        int imgSize = 40;
        QRect logoRect(cx - imgSize/2, cy - imgSize/2, imgSize, imgSize);
        p.drawPixmap(logoRect, logo.scaled(imgSize, imgSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    // Only schedule the next repaint if the splash is still visible.
    // fadeOutAndClose() sets m_animStopped=true to break this loop so
    // the 83fps timer doesn't keep firing after the window is hidden.
    if (!m_animStopped) {
        QTimer::singleShot(12, this, [=]() { update(); });
    }
}
