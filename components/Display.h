#pragma once
#include <QObject>
#include "../translations.h"
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QTimer>
#include <QString>
#include <QStringList>

class DisplayManager : public QObject {
    Q_OBJECT
public:
    explicit DisplayManager(QObject *parent = nullptr);
    
    void show();
    void hide();

private slots:
    void autoRefresh();
    void setBrightness(int value);
    void applyDisplayChange();

private:
    QString detectThemeImage();
    void updateDisplayDropdown();
    void updateBrightnessSlider();
    void updateThemeImage();
    QStringList getMonitorModes();
    QPair<int, QStringList> getCurrentMonitorLine();
    
    QWidget *displayBox;
    QLabel *desktopLabel;
    QLabel *desktopImageLabel;
    QLabel *resolutionLabel;
    QComboBox *displayDropdown;
    QLabel *brightnessLabel;
    QSlider *brightnessSlider;
    QPushButton *applyBtn;
    QTimer *refreshTimer;
    QString monitorName;
}; 