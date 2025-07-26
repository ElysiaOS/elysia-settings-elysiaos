#include "Network.h"
#include <QDir>
#include <QFont>
#include <QProcess>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>

WifiScanner::WifiScanner(QObject *parent) : QThread(parent) {}

void WifiScanner::run() {
    QStringList networks;
    QString connected = "None";
    bool wifiEnabled = false;
    
    try {
        QProcess process;
        process.start("nmcli", QStringList() << "radio" << "wifi");
        if (process.waitForFinished()) {
            QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed().toLower();
            wifiEnabled = (output == "enabled");
        }
        
        if (!wifiEnabled) {
            emit wifiScanned(networks, connected, wifiEnabled);
            return;
        }
        
        QProcess wifiProcess;
        wifiProcess.start("nmcli", QStringList() << "-t" << "-f" << "active,ssid" << "dev" << "wifi");
        if (wifiProcess.waitForFinished()) {
            QString output = QString::fromUtf8(wifiProcess.readAllStandardOutput()).trimmed();
            QStringList lines = output.split('\n', Qt::SkipEmptyParts);
            
            for (const QString &line : lines) {
                if (line.startsWith("yes:")) {
                    connected = line.split(":").value(1, "None");
                }
                networks.append(line);
            }
        }
    } catch (...) {
        wifiEnabled = false;
        connected = "Error";
    }
    
    emit wifiScanned(networks, connected, wifiEnabled);
}

Network::Network(QObject *parent) : QObject(parent) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    
    initNetworkUI();
    
    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(5000);
    connect(refreshTimer, &QTimer::timeout, this, &Network::refreshWifi);
    refreshTimer->start();
    
    scanner = new WifiScanner(this);
    connect(scanner, &WifiScanner::wifiScanned, this, &Network::updateWifiUI);
    
    refreshWifi();
}

void Network::initNetworkUI() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    
    networkBox = new QWidget(parentWidget);
    networkBox->setGeometry(500, 100, 600, 500);
    networkBox->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.05);"
        "border: 2px solid rgba(255, 255, 255, 0.15);"
        "border-radius: 26px;"
    );
    networkBox->setVisible(false);
    
    layout = new QVBoxLayout(networkBox);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);
    
    currentLabel = new QLabel("Connected to: Loading...", networkBox);
    QFont currentFont("ElysiaOSNew", 16, QFont::Bold);
    currentLabel->setFont(currentFont);
    currentLabel->setStyleSheet(
        "color: white;"
        "background: transparent;"
        "border: none;"
        "margin: 0px;"
        "padding: 0px;"
    );
    currentLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(currentLabel);
    
    disconnectButton = new QPushButton("Disconnect", networkBox);
    disconnectButton->setFixedHeight(40);
    disconnectButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #ff5555;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #ff7777;"
        "}"
    );
    connect(disconnectButton, &QPushButton::clicked, this, &Network::disconnectWifi);
    layout->addWidget(disconnectButton, 0, Qt::AlignCenter);
    
    enableButton = new QPushButton("Enable Wi-Fi", networkBox);
    enableButton->setFixedHeight(40);
    enableButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #55ff55;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 10px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #77ff77;"
        "}"
    );
    connect(enableButton, &QPushButton::clicked, this, &Network::enableWifi);
    layout->addWidget(enableButton, 0, Qt::AlignCenter);
    enableButton->setVisible(false);
    
    listLabel = new QLabel("Available Wi-Fi Networks:", networkBox);
    QFont listFont("ElysiaOSNew", 14);
    listLabel->setFont(listFont);
    listLabel->setStyleSheet(
        "color: white;"
        "background: transparent;"
        "border: none;"
        "margin: 0px;"
        "padding: 0px;"
    );
    layout->addWidget(listLabel, 0, Qt::AlignCenter);
    
    wifiContainer = new QFrame(networkBox);
    wifiContainer->setStyleSheet(
        "QFrame {"
        "    background-color: rgba(255, 255, 255, 0.05);"
        "    border: 2px solid rgba(255, 255, 255, 0.1);"
        "    border-radius: 20px;"
        "}"
    );
    
    QVBoxLayout *wifiLayout = new QVBoxLayout(wifiContainer);
    wifiLayout->setContentsMargins(10, 10, 10, 10);
    
    wifiList = new QListWidget(wifiContainer);
    wifiList->setStyleSheet(
        "QListWidget {"
        "    background: transparent;"
        "    border: none;"
        "    margin: 0px;"
        "    padding: 0px;"
        "    font-family: ElysiaOSNew;"
        "}"
    );
    wifiList->setFrameStyle(QFrame::NoFrame);
    wifiLayout->addWidget(wifiList);
    layout->addWidget(wifiContainer);
}

void Network::show() {
    refreshWifi();
    networkBox->setVisible(true);
}

void Network::hide() {
    networkBox->setVisible(false);
}

void Network::refreshWifi() {
    if (!scanner->isRunning()) {
        scanner->start();
    }
}

void Network::updateWifiUI(QStringList result, QString connected, bool wifiEnabled) {
    currentLabel->setText(QString("Connected to: %1").arg(connected));
    wifiList->clear();
    
    if (!wifiEnabled || connected == "None" || connected.isEmpty() || connected == "Error") {
        disconnectButton->setVisible(false);
        enableButton->setVisible(true);
    } else {
        disconnectButton->setVisible(true);
        enableButton->setVisible(false);
    }
    
    if (!wifiEnabled) {
        return;
    }
    
    QStringList savedSSIDs = getSavedConnections();
    
    for (const QString &line : result) {
        QStringList parts = line.split(":");
        if (parts.size() < 2) continue;
        
        QString active = parts[0];
        QString ssid = parts[1];
        
        if (active == "yes") continue;
        
        QWidget *itemWidget = new QWidget();
        QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(10, 5, 10, 5);
        
        QLabel *label = new QLabel(ssid);
        QFont labelFont("ElysiaOSNew", 12);
        label->setFont(labelFont);
        itemWidget->setStyleSheet(
            "color: white;"
            "background: transparent;"
            "border: none;"
            "margin: 0px;"
            "padding: 0px;"
        );
        
        QPushButton *connectBtn = new QPushButton("Connect");
        connectBtn->setFixedWidth(100);
        connectBtn->setStyleSheet(
            "QPushButton {"
            "    background-color: rgba(255, 255, 255, 0.15);"
            "    color: white;"
            "    border-radius: 8px;"
            "    padding: 5px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(255, 255, 255, 0.25);"
            "}"
        );
        
        // Use lambda to capture ssid
        connect(connectBtn, &QPushButton::clicked, [this, ssid]() {
            connectToWifi(ssid);
        });
        
        itemLayout->addWidget(label);
        itemLayout->addStretch();
        itemLayout->addWidget(connectBtn);
        
        if (savedSSIDs.contains(ssid)) {
            QPushButton *forgetBtn = new QPushButton("Forget");
            forgetBtn->setFixedWidth(100);
            forgetBtn->setStyleSheet(
                "QPushButton {"
                "    background-color: rgba(255, 255, 255, 0.15);"
                "    color: white;"
                "    border-radius: 8px;"
                "    padding: 5px;"
                "    font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "    background-color: rgba(255, 255, 255, 0.25);"
                "}"
            );
            
            connect(forgetBtn, &QPushButton::clicked, [this, ssid]() {
                forgetWifi(ssid);
            });
            
            itemLayout->addWidget(forgetBtn);
        }
        
        QListWidgetItem *listItem = new QListWidgetItem(wifiList);
        listItem->setSizeHint(itemWidget->sizeHint());
        wifiList->addItem(listItem);
        wifiList->setItemWidget(listItem, itemWidget);
    }
}

void Network::connectToWifi(const QString &ssid) {
    if (isSavedConnection(ssid)) {
        try {
            QProcess::startDetached("nmcli", QStringList() << "connection" << "up" << ssid);
            currentLabel->setText(QString("Connected to: %1").arg(ssid));
        } catch (...) {
            QMessageBox::warning(qobject_cast<QWidget*>(parent()), "Error", 
                               QString("Failed to connect to saved network %1.").arg(ssid));
        }
    } else {
        bool ok;
        QString password = QInputDialog::getText(networkBox, "Wi-Fi Password",
                                               QString("Enter password for %1:").arg(ssid),
                                               QLineEdit::Password, "", &ok);
        if (ok && !password.isEmpty()) {
            try {
                QProcess::startDetached("nmcli", QStringList() << "device" << "wifi" << "connect" << ssid << "password" << password);
                currentLabel->setText(QString("Connected to: %1").arg(ssid));
            } catch (...) {
                QMessageBox::warning(qobject_cast<QWidget*>(parent()), "Error", 
                                   QString("Failed to connect to %1.").arg(ssid));
            }
        }
    }
    
    refreshWifi();
}

void Network::forgetWifi(const QString &ssid) {
    if (isSavedConnection(ssid)) {
        try {
            QProcess::startDetached("nmcli", QStringList() << "connection" << "delete" << ssid);
            QMessageBox::information(qobject_cast<QWidget*>(parent()), "Success", 
                                   QString("Forgot network %1.").arg(ssid));
        } catch (...) {
            QMessageBox::warning(qobject_cast<QWidget*>(parent()), "Error", 
                               QString("Failed to forget network %1.").arg(ssid));
        }
    } else {
        QMessageBox::information(qobject_cast<QWidget*>(parent()), "Info", 
                               QString("No saved connection for %1.").arg(ssid));
    }
    refreshWifi();
}

QStringList Network::getSavedConnections() {
    QStringList connections;
    try {
        QProcess process;
        process.start("nmcli", QStringList() << "-t" << "-f" << "NAME" << "connection" << "show");
        if (process.waitForFinished()) {
            QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
            connections = output.split('\n', Qt::SkipEmptyParts);
        }
    } catch (...) {
        // Return empty list on error
    }
    return connections;
}

bool Network::isSavedConnection(const QString &ssid) {
    return getSavedConnections().contains(ssid);
}

void Network::disconnectWifi() {
    try {
        QProcess::startDetached("nmcli", QStringList() << "networking" << "off");
        QProcess::startDetached("nmcli", QStringList() << "networking" << "on");
        currentLabel->setText("Disconnected.");
    } catch (...) {
        currentLabel->setText("Failed to disconnect.");
    }
    refreshWifi();
}

void Network::enableWifi() {
    try {
        QProcess::startDetached("nmcli", QStringList() << "radio" << "wifi" << "on");
        currentLabel->setText("Wi-Fi Enabled.");
    } catch (...) {
        currentLabel->setText("Failed to enable Wi-Fi.");
    }
    refreshWifi();
} 