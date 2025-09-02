#include "Bluetooth.h"
#include "../translations.h"
#include <QDir>
#include <QFont>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QFrame>
#include <QDebug>

Bluetooth::Bluetooth(QObject *parent) : QObject(parent), scanning(false) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    initBluetoothUI();
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &Bluetooth::refreshDevices);
    refreshTimer->start(5000);
}

void Bluetooth::initBluetoothUI() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    bluetoothBox = new QWidget(parentWidget);
    bluetoothBox->setGeometry(500, 100, 600, 500);
    bluetoothBox->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.05);"
        "border: 2px solid rgba(255, 255, 255, 0.15);"
        "border-radius: 26px;"
    );
    bluetoothBox->setVisible(false);
    mainLayout = new QVBoxLayout(bluetoothBox);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);
    statusLabel = new QLabel(QString(Translations::tr("BLUETOOTH_STATUS")).arg(Translations::tr("BLUETOOTH_OFF")), bluetoothBox);
    statusLabel->setFont(QFont("ElysiaOSNew", 16, QFont::Bold));
    statusLabel->setStyleSheet("color: white; background: transparent; border: none;");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);
    buttonLayout = new QHBoxLayout();
    toggleBtn = new QPushButton(Translations::tr("TOGGLE_BLUETOOTH"), bluetoothBox);
    scanBtn = new QPushButton(Translations::tr("SCAN_DEVICES"), bluetoothBox);
    for (QPushButton* btn : {toggleBtn, scanBtn}) {
        btn->setFixedHeight(40);
        btn->setStyleSheet(
            "QPushButton {"
            "background-color: #e5a7c6;"
            "color: white;"
            "border: none;"
            "border-radius: 10px;"
            "font-weight: bold;"
            "font-size: 14px;"
            "}"
            "QPushButton:hover {"
            "background-color: #edcee3;"
            "}"
        );
    }
    connect(toggleBtn, &QPushButton::clicked, this, &Bluetooth::toggleBluetooth);
    connect(scanBtn, &QPushButton::clicked, this, &Bluetooth::toggleScan);
    buttonLayout->addWidget(toggleBtn);
    buttonLayout->addWidget(scanBtn);
    mainLayout->addLayout(buttonLayout);
    listLabel = new QLabel(Translations::tr("DEVICES"), bluetoothBox);
    listLabel->setFont(QFont("ElysiaOSNew", 14));
    listLabel->setStyleSheet("color: white; background: transparent; border: none;");
    mainLayout->addWidget(listLabel, 0, Qt::AlignCenter);
    deviceContainer = new QFrame(bluetoothBox);
    deviceContainer->setStyleSheet(
        "QFrame {"
        "background-color: rgba(255, 255, 255, 0.05);"
        "border: 2px solid rgba(255, 255, 255, 0.1);"
        "border-radius: 20px;"
        "}"
    );
    QVBoxLayout *deviceLayout = new QVBoxLayout(deviceContainer);
    deviceLayout->setContentsMargins(10, 10, 10, 10);
    deviceList = new QListWidget(deviceContainer);
    deviceList->setStyleSheet(
        "QListWidget {"
        "background: transparent;"
        "border: none;"
        "font-family: ElysiaOSNew;"
        "color: white;"
        "}"
    );
    deviceList->setFrameStyle(QFrame::NoFrame);
    deviceLayout->addWidget(deviceList);
    mainLayout->addWidget(deviceContainer);
    refreshDevices();
}

void Bluetooth::show() {
    refreshDevices();
    bluetoothBox->setVisible(true);
    refreshTimer->start(5000);
}

void Bluetooth::hide() {
    bluetoothBox->setVisible(false);
    refreshTimer->stop();
}

void Bluetooth::toggleBluetooth() {
    bool state = getBluetoothPowerState();
    QProcess proc;
    proc.start("bluetoothctl", QStringList() << "power" << (state ? "off" : "on"));
    proc.waitForFinished();
    refreshDevices();
}

bool Bluetooth::getBluetoothPowerState() {
    QProcess proc;
    proc.start("bluetoothctl", QStringList() << "show");
    proc.waitForFinished();
    QString output = QString::fromUtf8(proc.readAllStandardOutput());
    for (const QString &line : output.split('\n')) {
        if (line.contains("Powered:")) {
            return line.toLower().contains("yes");
        }
    }
    return false;
}

void Bluetooth::toggleScan() {
    if (scanning) {
        scanBtn->setText(Translations::tr("SCAN_DEVICES"));
        scanning = false;
        return;
    }
            scanBtn->setText(Translations::tr("SCAN_DEVICES"));
    scanBtn->setEnabled(false);
    scanning = true;
    QProcess::execute("bluetoothctl", QStringList() << "--timeout" << "3" << "scan" << "on");
    QProcess::execute("bluetoothctl", QStringList() << "--timeout" << "2" << "scan" << "off");
    scanning = false;
            scanBtn->setText(Translations::tr("SCAN_DEVICES"));
    scanBtn->setEnabled(true);
    refreshDevices();
}

void Bluetooth::refreshDevices() {
    bool state = getBluetoothPowerState();
    statusLabel->setText(QString("Bluetooth: %1").arg(state ? "On" : "Off"));
    deviceList->clear();
    if (!state) return;
    QProcess proc;
    proc.start("bluetoothctl", QStringList() << "devices");
    proc.waitForFinished();
    QStringList output = QString::fromUtf8(proc.readAllStandardOutput()).split('\n', Qt::SkipEmptyParts);
    QSet<QString> seen;
    for (const QString &line : output) {
        if (line.startsWith("Device")) {
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 3) {
                QString mac = parts[1];
                QString name = parts.mid(2).join(' ');
                if (seen.contains(mac)) continue;
                seen.insert(mac);
                // Check if device is connected
                QProcess infoProc;
                infoProc.start("bluetoothctl", QStringList() << "info" << mac);
                infoProc.waitForFinished();
                QString infoOut = QString::fromUtf8(infoProc.readAllStandardOutput());
                bool isConnected = false;
                for (const QString &infoLine : infoOut.split('\n')) {
                    if (infoLine.trimmed().startsWith("Connected:")) {
                        isConnected = infoLine.contains("yes");
                        break;
                    }
                }
                QWidget *itemWidget = new QWidget();
                QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
                itemLayout->setContentsMargins(10, 5, 10, 5);
                QLabel *label = new QLabel(name);
                label->setFont(QFont("ElysiaOSNew", 12));
                itemWidget->setStyleSheet("color: white; background: transparent; border: none;");
                QPushButton *actionBtn = nullptr;
                if (!isConnected) {
                    actionBtn = new QPushButton(Translations::tr("CONNECT"));
                    actionBtn->setFixedWidth(100);
                    actionBtn->setStyleSheet(
                        "QPushButton {"
                        "background-color: rgba(255, 255, 255, 0.15);"
                        "color: white;"
                        "font-size: 10px;"
                        "border-radius: 8px;"
                        "padding: 5px;"
                        "font-weight: bold;"
                        "}"
                        "QPushButton:hover {"
                        "background-color: rgba(255, 255, 255, 0.25);"
                        "}"
                    );
                    connect(actionBtn, &QPushButton::clicked, this, [this, mac]() {
                        QProcess::execute("bluetoothctl", QStringList() << "pair" << mac);
                        QProcess::execute("bluetoothctl", QStringList() << "connect" << mac);
                        QMessageBox::information(bluetoothBox, Translations::tr("SUCCESS"), QString(Translations::tr("CONNECTED_TO")).arg(mac));
                        refreshDevices();
                    });
                } else {
                    actionBtn = new QPushButton(Translations::tr("DISCONNECT"));
                    actionBtn->setFixedWidth(100);
                    actionBtn->setStyleSheet(
                        "QPushButton {"
                        "background-color: rgba(255, 255, 255, 0.15);"
                        "color: white;"
                        "font-size: 10px;"
                        "border-radius: 8px;"
                        "padding: 5px;"
                        "font-weight: bold;"
                        "}"
                        "QPushButton:hover {"
                        "background-color: rgba(255, 0, 0, 0.25);"
                        "}"
                    );
                    connect(actionBtn, &QPushButton::clicked, this, [this, mac]() {
                        QProcess::execute("bluetoothctl", QStringList() << "disconnect" << mac);
                        QMessageBox::information(bluetoothBox, Translations::tr("DISCONNECTED"), QString(Translations::tr("DISCONNECTED_FROM")).arg(mac));
                        refreshDevices();
                    });
                }
                QPushButton *forgetBtn = new QPushButton(Translations::tr("FORGET"));
                forgetBtn->setFixedWidth(100);
                forgetBtn->setStyleSheet(
                    "QPushButton {"
                    "background-color: rgba(255, 0, 0, 0.15);"
                    "color: white;"
                    "font-size: 10px;"
                    "border-radius: 8px;"
                    "padding: 5px;"
                    "font-weight: bold;"
                    "}"
                    "QPushButton:hover {"
                    "background-color: rgba(255, 0, 0, 0.3);"
                    "}"
                );
                connect(forgetBtn, &QPushButton::clicked, this, [this, mac]() {
                    QProcess::execute("bluetoothctl", QStringList() << "remove" << mac);
                    QMessageBox::information(bluetoothBox, Translations::tr("REMOVED"), QString(Translations::tr("FORGOT_DEVICE")).arg(mac));
                    refreshDevices();
                });
                itemLayout->addWidget(label);
                itemLayout->addStretch();
                if (actionBtn) itemLayout->addWidget(actionBtn);
                itemLayout->addWidget(forgetBtn);
                QListWidgetItem *listItem = new QListWidgetItem(deviceList);
                listItem->setSizeHint(itemWidget->sizeHint());
                deviceList->addItem(listItem);
                deviceList->setItemWidget(listItem, itemWidget);
            }
        }
    }
}

void Bluetooth::connectDevice() {}
void Bluetooth::forgetDevice() {} 