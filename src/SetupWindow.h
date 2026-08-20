#pragma once

#include <QWidget>

class DownloadManager;

class SetupWindow : public QWidget {
    Q_OBJECT

public:
    explicit SetupWindow(QWidget* parent = nullptr);
    ~SetupWindow() = default;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void onProgressChanged(int percent, const QString& status);
    void onDownloadComplete(bool success, const QString& message);
    
    DownloadManager* m_downloadManager;
    int m_progress = 0;
    QString m_status = "Initializing...";
    bool m_isComplete = false;
    QString m_finalMessage;
};
