#pragma once
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>
#include <QFrame>
#include <QTimer>
#include <QThread>
#include <QString>
#include <QStringList>

class WifiScanner : public QThread {
    Q_OBJECT
public:
    explicit WifiScanner(QObject *parent = nullptr);
    
signals:
    void wifiScanned(QStringList networks, QString connectedSSID, bool wifiEnabled);
    
protected:
    void run() override;
};

class Network : public QObject {
    Q_OBJECT
public:
    explicit Network(QObject *parent = nullptr);
    
    void show();
    void hide();

private slots:
    void refreshWifi();
    void updateWifiUI(QStringList result, QString connected, bool wifiEnabled);
    void connectToWifi(const QString &ssid);
    void forgetWifi(const QString &ssid);
    void disconnectWifi();
    void enableWifi();

private:
    void initNetworkUI();
    QStringList getSavedConnections();
    bool isSavedConnection(const QString &ssid);
    
    QWidget *networkBox;
    QVBoxLayout *layout;
    QLabel *currentLabel;
    QPushButton *disconnectButton;
    QPushButton *enableButton;
    QLabel *listLabel;
    QFrame *wifiContainer;
    QListWidget *wifiList;
    QTimer *refreshTimer;
    WifiScanner *scanner;
}; 