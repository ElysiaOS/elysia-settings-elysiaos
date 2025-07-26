#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVector>
#include <memory>
#include <functional>

// Include all component headers
#include "components/About.h"
#include "components/Applications.h"
#include "components/Battery.h"
#include "components/Bluetooth.h"
#include "components/Display.h"
#include "components/Network.h"
#include "components/Power.h"
#include "components/Sound.h"
#include "components/Storage.h"
#include "components/Wallpaper.h"

class AnimatedTile;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void handleBackClick();
    void enableAmphoreusMode();
    void enableElysianMode();
    void triggerDestruction();
    void closeApplication();
    
    // Section show methods
    void showAbout();
    void showDisplay();
    void showNetwork();
    void showSound();
    void showApplications();
    void showWallpaper();
    void showBluetooth();
    void showBattery();
    void showPower();
    void showStorage();
    void showSupport();
    void showUpdate();

private:
    void initBackButton();
    void initThemeButtons();
    void initTiles();
    void updateTileVisibility();
    void showSection(const QString &name);
    void hideTiles();
    void showTiles();
    void showBackButton();
    void hideBackButton();
    
    void addTile(const QString &theme, const QString &img, const QString &text, 
                 const QPoint &tilePos, const QPoint &labelPos, 
                 std::function<void()> handler = nullptr, bool isCar = false);

    QLabel *bgLabel;
    QPushButton *backBtn;
    QLabel *backLabel;
    QPushButton *themeButtonAmphoreus;
    QPushButton *themeButtonElysian;
    
    QString themeMode;
    QString currentView;
    bool destructionActive;
    QVector<AnimatedTile*> tiles;
    
    // Component managers
    std::unique_ptr<About> aboutManager;
    std::unique_ptr<Applications> applicationsManager;
    std::unique_ptr<Battery> batteryManager;
    std::unique_ptr<Bluetooth> bluetoothManager;
    std::unique_ptr<DisplayManager> displayManager;
    std::unique_ptr<Network> networkManager;
    std::unique_ptr<Power> powerManager;
    std::unique_ptr<Sound> soundManager;
    std::unique_ptr<Storage> storageManager;
    std::unique_ptr<Wallpaper> wallpaperManager;
}; 