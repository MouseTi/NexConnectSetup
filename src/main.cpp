#include "SplashScreen.h"
#include "SetupManager.h"

#include <QApplication>
#include <QMessageBox>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("NexConnectSetup");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("NexConnect");

    SplashScreen splash;
    splash.show();

    SetupManager* manager = new SetupManager(&app);

    // Update splash with status messages (optional - currently splash doesn't show text)
    QObject::connect(manager, &SetupManager::statusChanged, [](const QVariantMap& status) {
        // Could update splash screen status text here if we add that feature
    });

    QObject::connect(manager, &SetupManager::installationComplete, [&splash]() {
        // Fade out and close after successful installation
        QTimer::singleShot(800, &splash, &SplashScreen::fadeOutAndClose);
    });

    QObject::connect(manager, &SetupManager::installationFailed, [&app](const QString& error) {
        QMessageBox::critical(nullptr, "Installation Failed", 
            "Failed to install NexConnect:\n\n" + error,
            QMessageBox::Ok);
        app.quit();
    });

    QObject::connect(&splash, &SplashScreen::done, &app, &QApplication::quit);

    // Start installation after a brief delay to show splash
    QTimer::singleShot(1000, manager, &SetupManager::startInstallation);

    return app.exec();
}
