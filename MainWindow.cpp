#include "MainWindow.h"
#include "AnimatedTile.h"
#include <QPixmap>
#include <QFont>
#include <QIcon>
#include <QSize>
#include <QDir>
#include <QProcess>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QApplication>

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

MainWindow::MainWindow(QWidget *parent) : QWidget(parent), themeMode("elysian"), 
    currentView(""), destructionActive(false) {
    setWindowTitle("Elysia Settings");
    setFixedSize(1600, 800);
    setMouseTracking(true);

    bgLabel = new QLabel(this);
    QPixmap bgPixmap(QDir::homePath() + "/.config/Elysia/assets/assets/background.png");
    bgLabel->setPixmap(bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    bgLabel->setGeometry(0, 0, width(), height());

    initBackButton();
    initThemeButtons();
    initTiles();

    // Initialize component managers
    aboutManager = std::make_unique<About>(this);
    applicationsManager = std::make_unique<Applications>(this);
    batteryManager = std::make_unique<Battery>(this);
    bluetoothManager = std::make_unique<Bluetooth>(this);
    displayManager = std::make_unique<DisplayManager>(this);
    networkManager = std::make_unique<Network>(this);
    powerManager = std::make_unique<Power>(this);
    soundManager = std::make_unique<Sound>(this);
    storageManager = std::make_unique<Storage>(this);
    wallpaperManager = std::make_unique<Wallpaper>(this);
}

void MainWindow::initBackButton() {
    backBtn = new QPushButton(this);
    backBtn->setIcon(QIcon(QDir::homePath() + "/.config/Elysia/assets/assets/back.png"));
    backBtn->setIconSize(QSize(159, 66));
    backBtn->setFixedSize(159, 66);
    backBtn->setVisible(false);
    backBtn->setStyleSheet("border: none; background: transparent;");
    backBtn->move(20, 2);
    connect(backBtn, &QPushButton::clicked, this, &MainWindow::handleBackClick);

    backLabel = new QLabel("BACK", this);
    QFont font("ElysiaOSNew12", 18, QFont::Bold);
    backLabel->setFont(font);
    backLabel->setStyleSheet("color: white; background: transparent;");
    backLabel->adjustSize();
    backLabel->move(backBtn->x() + backBtn->width() - 125,
                    backBtn->y() + backBtn->height() / 2 - backLabel->height() / 2);
    backLabel->setVisible(false);
}

void MainWindow::initThemeButtons() {
    themeButtonAmphoreus = new QPushButton("Amphoreus mode", this);
    themeButtonAmphoreus->setGeometry(width() - 250, 20, 200, 40);
    themeButtonAmphoreus->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(255, 255, 255, 0.1);"
        "    color: white;"
        "    font-weight: bold;"
        "    border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(255, 255, 255, 0.2);"
        "}"
    );
    connect(themeButtonAmphoreus, &QPushButton::clicked, this, &MainWindow::enableAmphoreusMode);

    themeButtonElysian = new QPushButton("Elysian Realm mode", this);
    themeButtonElysian->setGeometry(width() - 250, 20, 200, 40);
    themeButtonElysian->setStyleSheet(
        "QPushButton {"
        "    background-color: rgba(255, 255, 255, 0.1);"
        "    color: white;"
        "    font-weight: bold;"
        "    border-radius: 10px;"
        "}"
        "QPushButton:hover {"
        "    background-color: rgba(255, 255, 255, 0.2);"
        "}"
    );
    connect(themeButtonElysian, &QPushButton::clicked, this, &MainWindow::enableElysianMode);
    themeButtonElysian->hide();
}

void MainWindow::initTiles() {
    // Elysian Realm Tiles
    addTile("elysian", "about.png", "ABOUT", QPoint(750, 240), QPoint(764, 210), 
            [this]() { showAbout(); });
    addTile("elysian", "display.png", "DISPLAY", QPoint(880, 130), QPoint(885, 115), 
            [this]() { showDisplay(); });
    addTile("elysian", "network.png", "NETWORK", QPoint(940, 380), QPoint(930, 370), 
            [this]() { showNetwork(); });
    addTile("elysian", "update.png", "UPDATES", QPoint(1080, 183), QPoint(1075, 180), 
            [this]() { showUpdate(); });
    addTile("elysian", "snake.png", "STORAGE", QPoint(1195, 340), QPoint(1200, 330), 
            [this]() { showStorage(); });
    addTile("elysian", "apps.png", "HYPRLAND", QPoint(1290, 162), QPoint(1280, 160), 
            [this]() { showApplications(); });
    addTile("elysian", "support.png", "SUPPORT", QPoint(1340, 410), QPoint(1330, 405), 
            [this]() { showSupport(); });
    addTile("elysian", "bluetooth.png", "BLUETOOTH", QPoint(630, 95), QPoint(620, 100), 
            [this]() { showBluetooth(); });
    addTile("elysian", "sound.png", "SOUND", QPoint(610, 395), QPoint(625, 380), 
            [this]() { showSound(); });
    addTile("elysian", "power.png", "POWER", QPoint(480, 210), QPoint(495, 205), 
            [this]() { showPower(); });
    addTile("elysian", "battery.png", "BATTERY", QPoint(325, 105), QPoint(337, 100), 
            [this]() { showBattery(); });
    addTile("elysian", "wallpaper.png", "APPEARANCE", QPoint(366, 355), QPoint(349, 340), 
            [this]() { showWallpaper(); });
    addTile("elysian", "car.png", "CAR", QPoint(190, 290), QPoint(225, 290), 
            [this]() { triggerDestruction(); }, true);

    // Amphoreus Tiles
    addTile("amphoreus", "about3.png", "", QPoint(1400, 190), QPoint(852, 355), 
            [this]() { showAbout(); });
    addTile("amphoreus", "display2.png", "DISPLAY", QPoint(320, 595), QPoint(320, 715), 
            [this]() { showDisplay(); });
    addTile("amphoreus", "network2.png", "NETWORK", QPoint(320, 395), QPoint(320, 510), 
            [this]() { showNetwork(); });
    addTile("amphoreus", "update2.png", "UPDATES", QPoint(495, 595), QPoint(495, 715), 
            [this]() { showUpdate(); });
    addTile("amphoreus", "snake2.png", "STORAGE", QPoint(500, 395), QPoint(500, 510), 
            [this]() { showStorage(); });
    addTile("amphoreus", "apps2.png", "HYPRLAND", QPoint(850, 395), QPoint(850, 510), 
            [this]() { showApplications(); });
    addTile("amphoreus", "support2.png", "SUPPORT", QPoint(1030, 595), QPoint(1030, 715), 
            [this]() { showSupport(); });
    addTile("amphoreus", "bluetooth2.png", "BLUETOOTH", QPoint(675, 595), QPoint(675, 715), 
            [this]() { showBluetooth(); });
    addTile("amphoreus", "sound2.png", "SOUND", QPoint(1030, 395), QPoint(1030, 510), 
            [this]() { showSound(); });
    addTile("amphoreus", "power2.png", "POWER", QPoint(850, 595), QPoint(850, 715), 
            [this]() { showPower(); });
    addTile("amphoreus", "battery2.png", "BATTERY", QPoint(675, 395), QPoint(675, 510), 
            [this]() { showBattery(); });
    addTile("amphoreus", "wallpaper2.png", "APPEARANCE", QPoint(1210, 395), QPoint(1210, 510), 
            [this]() { showWallpaper(); });
    addTile("amphoreus", "car2.png", "CAR", QPoint(1210, 595), QPoint(1210, 715), 
            [this]() { triggerDestruction(); }, true);
}

void MainWindow::addTile(const QString &theme, const QString &img, const QString &text, 
                        const QPoint &tilePos, const QPoint &labelPos, 
                        std::function<void()> handler, bool isCar) {
    AnimatedTile *tile = new AnimatedTile(this, img, text, tilePos, labelPos, "rotate", handler, isCar);
    tile->theme = theme;
    bool visible = (theme == themeMode);
    tile->setVisible(visible);
    if (tile->getLabel()) tile->getLabel()->setVisible(visible);
    if (tile->getGlass()) tile->getGlass()->setVisible(visible);
    tiles.append(tile);
}

void MainWindow::updateTileVisibility() {
    for (AnimatedTile *tile : tiles) {
        bool visible = (tile->theme == themeMode);
        tile->setVisible(visible);
        if (tile->getLabel()) tile->getLabel()->setVisible(visible);
        if (tile->getGlass()) tile->getGlass()->setVisible(visible);
    }
}

void MainWindow::enableAmphoreusMode() {
    themeMode = "amphoreus";
    themeButtonAmphoreus->hide();
    themeButtonElysian->show();
    QPixmap bgPixmap(QDir::homePath() + "/.config/Elysia/assets/assets/background2.png");
    bgLabel->setPixmap(bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    updateTileVisibility();
}

void MainWindow::enableElysianMode() {
    themeMode = "elysian";
    themeButtonElysian->hide();
    themeButtonAmphoreus->show();
    QPixmap bgPixmap(QDir::homePath() + "/.config/Elysia/assets/assets/background.png");
    bgLabel->setPixmap(bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    updateTileVisibility();
}

void MainWindow::handleBackClick() {
    if (!currentView.isEmpty()) {
        // Hide current manager
        if (currentView == "about") aboutManager->hide();
        else if (currentView == "display") displayManager->hide();
        else if (currentView == "network") networkManager->hide();
        else if (currentView == "sound") soundManager->hide();
        else if (currentView == "applications") applicationsManager->hide();
        else if (currentView == "wallpaper") wallpaperManager->hide();
        else if (currentView == "bluetooth") bluetoothManager->hide();
        else if (currentView == "battery") batteryManager->hide();
        else if (currentView == "power") powerManager->hide();
        else if (currentView == "storage") storageManager->hide();
        currentView = "";
    }
    updateTileVisibility();
    showTiles();
    hideBackButton();
    if (themeMode == "elysian") {
        QPixmap bgPixmap(QDir::homePath() + "/.config/Elysia/assets/assets/background.png");
        bgLabel->setPixmap(bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        themeButtonAmphoreus->show();
        themeButtonElysian->hide();
    } else {
        QPixmap bgPixmap(QDir::homePath() + "/.config/Elysia/assets/assets/background2.png");
        bgLabel->setPixmap(bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        themeButtonElysian->show();
        themeButtonAmphoreus->hide();
    }
}

void MainWindow::showSection(const QString &name) {
    if (!destructionActive) {
        currentView = name;
        hideTiles();
        // Show the appropriate manager
        if (name == "about") aboutManager->show();
        else if (name == "display") displayManager->show();
        else if (name == "network") networkManager->show();
        else if (name == "sound") soundManager->show();
        else if (name == "applications") applicationsManager->show();
        else if (name == "wallpaper") wallpaperManager->show();
        else if (name == "bluetooth") bluetoothManager->show();
        else if (name == "battery") batteryManager->show();
        else if (name == "power") powerManager->show();
        else if (name == "storage") storageManager->show();
        showBackButton();
        themeButtonAmphoreus->hide();
        themeButtonElysian->hide();
        if (themeMode == "amphoreus") {
            QPixmap bgPixmap(QDir::homePath() + "/.config/Elysia/assets/assets/background3.png");
            bgLabel->setPixmap(bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        } else if (themeMode == "elysian") {
            QPixmap bgPixmap(QDir::homePath() + "/.config/Elysia/assets/assets/background4.png");
            bgLabel->setPixmap(bgPixmap.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        }
    }
}

// Section show methods
void MainWindow::showAbout() { showSection("about"); }
void MainWindow::showDisplay() { showSection("display"); }
void MainWindow::showNetwork() { showSection("network"); }
void MainWindow::showSound() { showSection("sound"); }
void MainWindow::showApplications() { showSection("applications"); }
void MainWindow::showWallpaper() { showSection("wallpaper"); }
void MainWindow::showBluetooth() { showSection("bluetooth"); }
void MainWindow::showBattery() { showSection("battery"); }
void MainWindow::showPower() { showSection("power"); }
void MainWindow::showStorage() { showSection("storage"); }

void MainWindow::showSupport() {
    if (!destructionActive) {
        QDesktopServices::openUrl(QUrl("https://ko-fi.com/matsuko3"));
    }
}

void MainWindow::showUpdate() {
    if (!destructionActive) {
        try {
            QProcess::startDetached("/usr/bin/elysia-updater.sh");
        } catch (...) {
            QMessageBox::critical(this, "Error", "Failed to run update script");
        }
    }
}

void MainWindow::triggerDestruction() {
    if (destructionActive) return;
    destructionActive = true;
    
    for (int i = 0; i < tiles.size(); ++i) {
        QTimer::singleShot(i * 150, [this, i]() {
            if (i < tiles.size()) {
                tiles[i]->destroyWithFall();
            }
        });
    }
    
    QTimer::singleShot(tiles.size() * 150 + 2000, this, &MainWindow::closeApplication);
}

void MainWindow::closeApplication() {
    QApplication::instance()->quit();
}

void MainWindow::hideTiles() {
    for (AnimatedTile *tile : tiles) {
        tile->hide();
        if (tile->getLabel()) tile->getLabel()->hide();
        if (tile->getGlass()) tile->getGlass()->hide();
    }
}

void MainWindow::showTiles() {
    for (AnimatedTile *tile : tiles) {
        if (!tile->isHidden() && !tile->isDestroyed && tile->theme == themeMode) {
            tile->show();
            if (tile->getLabel()) tile->getLabel()->show();
            if (tile->getGlass()) tile->getGlass()->show();
        }
    }
}

void MainWindow::showBackButton() {
    backBtn->setVisible(true);
    backLabel->setVisible(true);
}

void MainWindow::hideBackButton() {
    backBtn->setVisible(false);
    backLabel->setVisible(false);
} 

void MainWindow::launchSection(const QString &sectionName) {
    // Map of valid section names to their corresponding show methods
    QMap<QString, std::function<void()>> sectionMap = {
        {"about", [this]() { showAbout(); }},
        {"display", [this]() { showDisplay(); }},
        {"network", [this]() { showNetwork(); }},
        {"sound", [this]() { showSound(); }},
        {"applications", [this]() { showApplications(); }},
        {"apps", [this]() { showApplications(); }},  // Alias
        {"hyprland", [this]() { showApplications(); }},  // Alias
        {"wallpaper", [this]() { showWallpaper(); }},
        {"appearance", [this]() { showWallpaper(); }},  // Alias
        {"bluetooth", [this]() { showBluetooth(); }},
        {"battery", [this]() { showBattery(); }},
        {"power", [this]() { showPower(); }},
        {"storage", [this]() { showStorage(); }},
        {"support", [this]() { showSupport(); }},
        {"update", [this]() { showUpdate(); }},
        {"updates", [this]() { showUpdate(); }}  // Alias
    };
    
    // Check if the section exists and launch it
    if (sectionMap.contains(sectionName)) {
        // Use QTimer::singleShot to ensure the window is fully initialized first
        QTimer::singleShot(100, this, sectionMap[sectionName]);
    } else {
        // Invalid section name - just show the main interface
        qWarning() << "Unknown section:" << sectionName;
        qWarning() << "Valid sections: about, display, network, sound, applications, wallpaper, bluetooth, battery, power, storage, support, update";
    }
}