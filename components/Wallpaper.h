#pragma once
#include <QObject>
#include "../translations.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QTimer>
#include <QStringList>
#include <QMap>

class Wallpaper : public QObject {
    Q_OBJECT
public:
    explicit Wallpaper(QObject *parent = nullptr);
    void show();
    void hide();

private slots:
    void selectTheme();
    void previousWallpaper();
    void nextWallpaper();
    void applyWallpaper();
    void browseWallpapers();
    void checkThemeUpdate();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    QString frameToTheme(QObject *frameObj);
    void initWallpaperUI();
    void loadWallpapers();
    void updateWallpaperPreview();
    QString detectTheme();

    QWidget *wallpaperBox;
    QLabel *wallpaperPreview;
    QPushButton *leftBtn;
    QPushButton *rightBtn;
    QPushButton *applyBtn;
    QPushButton *browseBtn;
    QMap<QString, QPair<QFrame*, QLabel*>> themeBoxes;
    QStringList wallpapers;
    QString themeSelected;
    QString wallpaperSelected;
    int currentIndex;
    QTimer *themeTimer;
    QString lastDetectedTheme;
}; 