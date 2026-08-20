#include <QApplication>
#include "SplashScreen.h"
#include "SetupWindow.h"
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setOrganizationName("NexConnect");
    app.setApplicationName("NexConnect Setup");
    app.setApplicationVersion("1.0.0");

    // Show splash screen
    SplashScreen* splash = new SplashScreen();
    splash->show();

    // After splash animation, show setup window
    SetupWindow* setupWindow = new SetupWindow();
    
    QObject::connect(splash, &SplashScreen::done, [setupWindow]() {
        setupWindow->show();
    });

    // Fade out splash after 2 seconds and show setup
    QTimer::singleShot(2000, splash, &SplashScreen::fadeOutAndClose);

    return app.exec();
}
