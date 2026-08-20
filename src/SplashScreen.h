#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget* parent = nullptr);
    ~SplashScreen() = default;

signals:
    void done();

public slots:
    void fadeOutAndClose();

private:
    void paintEvent(QPaintEvent* event) override;
    void drawLoading(QPainter& p, const QRect& rect);
    bool m_animStopped = false; // set to true after fadeOutAndClose() to
                                // stop the 12ms repaint loop
};

#endif // SPLASHSCREEN_H
