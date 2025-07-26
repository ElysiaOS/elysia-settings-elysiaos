#include "Wallpaper.h"
#include <QDir>
#include <QFont>
#include <QPixmap>
#include <QProcess>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QDebug>
#include <QEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QFrame>
#include <QStackedLayout>
#include <QPushButton>
#include <QTimer>

Wallpaper::Wallpaper(QObject *parent) : QObject(parent), currentIndex(0) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    themeTimer = new QTimer(this);
    connect(themeTimer, &QTimer::timeout, this, &Wallpaper::checkThemeUpdate);
    initWallpaperUI();
}

void Wallpaper::initWallpaperUI() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;

    wallpaperBox = new QWidget(parentWidget);
    wallpaperBox->setGeometry(380, 100, 900, 600); // More compact, centered
    wallpaperBox->setStyleSheet(
        "background-color: transparent;"
        "border: 2px solid rgba(255, 255, 255, 0.10);"
        "border-radius: 32px;"
        "box-shadow: 0 8px 32px rgba(0,0,0,0.18);"
    );
    wallpaperBox->setVisible(false);

    QVBoxLayout *mainLayout = new QVBoxLayout(wallpaperBox);
    mainLayout->setContentsMargins(60, 36, 60, 36);
    mainLayout->setSpacing(44); // More vertical space

    // THEME SELECTION AREA
    QFrame *themeFrame = new QFrame(wallpaperBox);
    themeFrame->setStyleSheet("background: transparent; border: none; border-radius: 0px;");
    QVBoxLayout *themeFrameLayout = new QVBoxLayout(themeFrame);
    themeFrameLayout->setContentsMargins(0, 0, 0, 0);
    themeFrameLayout->setSpacing(10);

    QLabel *themeLabel = new QLabel("Theme", themeFrame);
    themeLabel->setFont(QFont("ElysiaOSNew", 22, QFont::Bold));
    themeLabel->setStyleSheet("color: #fff; background: transparent; border: none; letter-spacing: 2px;");
    themeLabel->setAlignment(Qt::AlignCenter);
    themeFrameLayout->addWidget(themeLabel);

    QHBoxLayout *themeContainer = new QHBoxLayout();
    themeContainer->setSpacing(48); // More space between theme icons
    themeContainer->setAlignment(Qt::AlignCenter);

    for (const QString &theme : {"Light", "Dark"}) {
        QFrame *frame = new QFrame(themeFrame);
        frame->setFixedSize(110, 110);
        // Default: no border, no background
        frame->setStyleSheet(
            "background: transparent; border: none; border-radius: 18px;"
        );
        frame->setCursor(Qt::PointingHandCursor);
        frame->setToolTip("Switch to " + theme + " theme");
        QVBoxLayout *frameLayout = new QVBoxLayout(frame);
        frameLayout->setContentsMargins(0, 0, 0, 0);
        frameLayout->setSpacing(0);
        QLabel *imageLabel = new QLabel();
        imageLabel->setAlignment(Qt::AlignCenter);
        imageLabel->setFixedSize(110, 110);
        QString imagePath = QDir::homePath() + "/.config/Elysia/assets/assets/" + theme.toLower() + ".png";
        if (QFileInfo::exists(imagePath)) {
            QPixmap pixmap(imagePath);
            imageLabel->setPixmap(pixmap.scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imageLabel->setText("Missing");
        }
        imageLabel->setStyleSheet("background: transparent; border: none; margin: 0; padding: 0;");
        QLabel *tick = new QLabel("✔");
        tick->setStyleSheet("color: #e5a7c6; font-size: 18px; background: transparent; border: none; text-shadow: 0 2px 8px #0008;");
        tick->setAlignment(Qt::AlignTop | Qt::AlignRight);
        tick->setVisible(false);
        frameLayout->addWidget(imageLabel);
        frameLayout->addWidget(tick, 0, Qt::AlignTop | Qt::AlignRight);
        frame->setProperty("themeName", theme);
        frame->installEventFilter(this);
        themeBoxes[theme] = qMakePair(frame, tick);
        themeContainer->addWidget(frame);
    }
    themeFrameLayout->addLayout(themeContainer);
    mainLayout->addWidget(themeFrame, 0, Qt::AlignHCenter);

    // Add extra vertical space between theme and preview
    mainLayout->addSpacing(24);

    // WALLPAPER PREVIEW AREA
    QLabel *wallpaperLabel = new QLabel("Wallpaper", wallpaperBox);
    wallpaperLabel->setFont(QFont("ElysiaOSNew", 20, QFont::Bold));
    wallpaperLabel->setStyleSheet("color: #fff; background: transparent; border: none; letter-spacing: 2px;");
    wallpaperLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(wallpaperLabel);

    QHBoxLayout *previewLayout = new QHBoxLayout();
    previewLayout->setSpacing(36); // More space between arrows and preview
    previewLayout->setAlignment(Qt::AlignCenter);

    leftBtn = new QPushButton("⮜", wallpaperBox);
    leftBtn->setFixedSize(40, 40);
    leftBtn->setStyleSheet(
        "QPushButton { color: #fff; font-size: 24px; background: rgba(229,167,198,0.10); border: none; border-radius: 20px; }"
        "QPushButton:hover { background: #e5a7c6; color: #fff; }"
    );
    leftBtn->setToolTip("Previous wallpaper");
    connect(leftBtn, &QPushButton::clicked, this, &Wallpaper::previousWallpaper);
    previewLayout->addWidget(leftBtn, 0, Qt::AlignVCenter);

    wallpaperPreview = new QLabel("", wallpaperBox);
    wallpaperPreview->setFixedSize(420, 220);
    wallpaperPreview->setStyleSheet(
        "color: #fff; background: rgba(255,255,255,0.03); border-radius: 16px; border: none; margin: 0px; padding: 0px;"
    );
    wallpaperPreview->setAlignment(Qt::AlignCenter);
    previewLayout->addWidget(wallpaperPreview, 0, Qt::AlignVCenter);

    rightBtn = new QPushButton("⮞", wallpaperBox);
    rightBtn->setFixedSize(40, 40);
    rightBtn->setStyleSheet(
        "QPushButton { color: #fff; font-size: 24px; background: rgba(229,167,198,0.10); border: none; border-radius: 20px; }"
        "QPushButton:hover { background: #e5a7c6; color: #fff; }"
    );
    rightBtn->setToolTip("Next wallpaper");
    connect(rightBtn, &QPushButton::clicked, this, &Wallpaper::nextWallpaper);
    previewLayout->addWidget(rightBtn, 0, Qt::AlignVCenter);

    mainLayout->addLayout(previewLayout);

    // Add extra vertical space between preview and buttons
    mainLayout->addSpacing(32);

    // BUTTONS AREA
    browseBtn = new QPushButton("Browse", wallpaperBox);
    browseBtn->setFixedSize(140, 40);
    browseBtn->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e5a7c6, stop:1 #edcee3);"
        "    color: #fff;"
        "    font-weight: bold;"
        "    border: none;"
        "    padding: 6px 12px;"
        "    letter-spacing: 1px;"
        "    border-radius: 10px;"
        "    font-size: 15px;"
        "}"
        "QPushButton:hover {"
        "    background: #edcee3; color: #a87c4f;"
        "}"
    );
    browseBtn->setToolTip("Open wallpaper folder");
    connect(browseBtn, &QPushButton::clicked, this, &Wallpaper::browseWallpapers);
    // Place directly under the preview, left side
    browseBtn->move(280, 550);

    applyBtn = new QPushButton("Apply", wallpaperBox);
    applyBtn->setFixedSize(140, 40);
    applyBtn->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e5a7c6, stop:1 #edcee3);"
        "    color: #fff;"
        "    font-weight: bold;"
        "    border: none;"
        "    padding: 6px 12px;"
        "    letter-spacing: 1px;"
        "    border-radius: 10px;"
        "    font-size: 15px;"
        "}"
        "QPushButton:hover {"
        "    background: #edcee3; color: #a87c4f;"
        "}"
    );
    applyBtn->setToolTip("Set this wallpaper");
    connect(applyBtn, &QPushButton::clicked, this, &Wallpaper::applyWallpaper);
    // Place directly under the preview, right side
    applyBtn->move(480, 550);
}

void Wallpaper::show() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    if (parentWidget->metaObject()->indexOfMethod("hideTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideTiles");
    if (parentWidget->metaObject()->indexOfMethod("showBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showBackButton");
    wallpaperBox->setVisible(true);
    lastDetectedTheme = detectTheme();
    selectTheme();
    themeTimer->start(1000);
}

void Wallpaper::hide() {
    wallpaperBox->setVisible(false);
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    if (parentWidget->metaObject()->indexOfMethod("hideBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideBackButton");
    if (parentWidget->metaObject()->indexOfMethod("showTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showTiles");
    themeTimer->stop();
}

QString Wallpaper::detectTheme() {
    QProcess process;
    process.start("gsettings", QStringList() << "get" << "org.gnome.desktop.interface" << "color-scheme");
    process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput()).toLower();
    if (output.contains("light")) return "Light";
    return "Dark";
}

void Wallpaper::checkThemeUpdate() {
    QString newTheme = detectTheme();
    if (newTheme != lastDetectedTheme) {
        lastDetectedTheme = newTheme;
        selectTheme();
    }
}

void Wallpaper::selectTheme() {
    // Default to detected theme if not set
    if (themeSelected.isEmpty())
        themeSelected = lastDetectedTheme.isEmpty() ? detectTheme() : lastDetectedTheme;
    // Update tick and border
    for (auto it = themeBoxes.begin(); it != themeBoxes.end(); ++it) {
        bool isSelected = (it.key() == themeSelected);
        it.value().first->setStyleSheet(QString(
            "background: transparent; border: %1 solid #e5a7c6; border-radius: 18px; margin: 0px; padding: 0px;"
        ).arg(isSelected ? "3px" : "0px"));
        it.value().second->setVisible(isSelected);
    }
    loadWallpapers();
    // Run theme script if exists
    QString scriptPath = QDir::homePath() + "/.config/Elysia/" + themeSelected + "Theme.sh";
    if (QFileInfo::exists(scriptPath)) {
        QProcess::startDetached(scriptPath);
    }
}

void Wallpaper::loadWallpapers() {
    wallpapers.clear();
    QString folder = QDir::homePath() + "/.config/Elysia/wallpaper/" + themeSelected + "/";
    QDir dir(folder);
    if (dir.exists()) {
        QStringList filters;
        filters << "*.jpg" << "*.jpeg" << "*.png";
        wallpapers = dir.entryList(filters, QDir::Files, QDir::Name);
    }
    currentIndex = 0;
    updateWallpaperPreview();
}

void Wallpaper::updateWallpaperPreview() {
    if (wallpapers.isEmpty()) {
        wallpaperPreview->clear();
        wallpaperSelected.clear();
        return;
    }
    wallpaperSelected = wallpapers[currentIndex];
    QString path = QDir::homePath() + "/.config/Elysia/wallpaper/" + themeSelected + "/" + wallpaperSelected;
    if (QFileInfo::exists(path)) {
        QPixmap pix(path);
        QSize previewSize(420, 220);
        QPixmap scaled = pix.scaled(previewSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        // Center crop if needed
        int x = (scaled.width() - previewSize.width()) / 2;
        int y = (scaled.height() - previewSize.height()) / 2;
        QPixmap cropped = scaled.copy(x, y, previewSize.width(), previewSize.height());
        wallpaperPreview->setPixmap(cropped);
    } else {
        wallpaperPreview->setText("Missing");
    }
}

void Wallpaper::previousWallpaper() {
    if (!wallpapers.isEmpty()) {
        currentIndex = (currentIndex - 1 + wallpapers.size()) % wallpapers.size();
        updateWallpaperPreview();
    }
}

void Wallpaper::nextWallpaper() {
    if (!wallpapers.isEmpty()) {
        currentIndex = (currentIndex + 1) % wallpapers.size();
        updateWallpaperPreview();
    }
}

void Wallpaper::applyWallpaper() {
    if (wallpaperSelected.isEmpty()) return;
    QString path = QDir::homePath() + "/.config/Elysia/wallpaper/" + themeSelected + "/" + wallpaperSelected;
    if (QFileInfo::exists(path)) {
        QProcess::startDetached("swww", QStringList() << "img" << path << "--transition-type" << "grow" << "--transition-step" << "10" << "--transition-fps" << "60");
    }
}

void Wallpaper::browseWallpapers() {
    QString path = QDir::homePath() + "/.config/Elysia/wallpaper";
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

bool Wallpaper::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QString theme = frameToTheme(obj);
        if (!theme.isEmpty()) {
            themeSelected = theme;
            selectTheme();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}

QString Wallpaper::frameToTheme(QObject *frameObj) {
    for (auto it = themeBoxes.begin(); it != themeBoxes.end(); ++it) {
        if (it.value().first == frameObj)
            return it.key();
    }
    return QString();
} 