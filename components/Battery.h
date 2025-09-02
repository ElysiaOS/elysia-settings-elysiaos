#pragma once
#include <QObject>
#include <QWidget>
#include "../translations.h"
#include <QLabel>
#include <QTimer>
#include <QString>
#include <QMap>

class Battery : public QObject {
    Q_OBJECT
public:
    explicit Battery(QObject *parent = nullptr);
    void show();
    void hide();

private slots:
    void updateBatteryStatus();

private:
    void initBatteryUI();
    QString findBatteryDevice();
    QMap<QString, QString> parseBatteryInfo(const QString &device);
    
    QWidget *batteryBox;
    QString batteryDevice;
    QList<QLabel*> infoLabels;
    QTimer *updateTimer;
}; 