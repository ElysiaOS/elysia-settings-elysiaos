#pragma once
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QTimer>
#include <QString>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>

class Bluetooth : public QObject {
    Q_OBJECT
public:
    explicit Bluetooth(QObject *parent = nullptr);
    void show();
    void hide();

private slots:
    void toggleBluetooth();
    void toggleScan();
    void refreshDevices();
    void connectDevice();
    void forgetDevice();

private:
    void initBluetoothUI();
    bool getBluetoothPowerState();
    void updateDeviceList();

    QWidget *bluetoothBox;
    QLabel *statusLabel;
    QPushButton *toggleBtn;
    QPushButton *scanBtn;
    QLabel *listLabel;
    QFrame *deviceContainer;
    QListWidget *deviceList;
    QTimer *refreshTimer;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    bool scanning;
}; 