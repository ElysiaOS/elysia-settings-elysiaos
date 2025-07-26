#include "Storage.h"
#include <QDir>
#include <QFont>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QFileInfo>
#include <QRegularExpression>
#include <QPixmap>
#include <QDebug>

Storage::Storage(QObject *parent) : QObject(parent) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    driveWidgets.clear();
    initUI();
    setupTimer();
}

void Storage::initUI() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    storageBox = new QWidget(parentWidget);
    storageBox->setGeometry(455, 100, 700, 590);
    storageBox->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.05);"
        "border: 2px solid rgba(255, 255, 255, 0.15);"
        "border-radius: 30px;"
    );
    storageBox->setVisible(false);
    QVBoxLayout *mainLayout = new QVBoxLayout(storageBox);
    mainLayout->setContentsMargins(60, 60, 60, 60);
    mainLayout->setSpacing(20);
    QLabel *storageLabel = new QLabel("STORAGE", storageBox);
    storageLabel->setFont(QFont("ElysiaOSNew", 16));
    storageLabel->setStyleSheet("color: white; background: transparent; border: none;");
    storageLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(storageLabel);
    drivesContainer = new QVBoxLayout();
    drivesContainer->setSpacing(20);
    mainLayout->addLayout(drivesContainer);
}

void Storage::show() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    if (parentWidget->metaObject()->indexOfMethod("hideTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideTiles");
    if (parentWidget->metaObject()->indexOfMethod("showBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showBackButton");
    storageBox->setVisible(true);
    updateDisks();
    if (updateTimer) updateTimer->start();
}

void Storage::hide() {
    storageBox->setVisible(false);
    if (updateTimer) updateTimer->stop();
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    if (parentWidget->metaObject()->indexOfMethod("hideBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideBackButton");
    if (parentWidget->metaObject()->indexOfMethod("showTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showTiles");
}

void Storage::setupTimer() {
    updateTimer = new QTimer(this);
    updateTimer->setInterval(1000);
    connect(updateTimer, &QTimer::timeout, this, &Storage::updateDisks);
}

void Storage::updateDisks() {
    // Clear previous widgets
    for (QWidget *widget : driveWidgets) {
        widget->setParent(nullptr);
        widget->deleteLater();
    }
    driveWidgets.clear();

    // Get partition info using df -T -B1
    QProcess process;
    process.start("df", QStringList() << "-T" << "-B1");
    process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    if (lines.size() < 2) return;
    // Header: Filesystem Type 1B-blocks Used Available Use% Mounted on
    QStringList validTypes = {"ext4", "btrfs", "xfs", "vfat", "ntfs", "fuseblk", "exfat"};
    for (int i = 1; i < lines.size(); ++i) {
        QStringList cols = lines[i].split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (cols.size() < 7) continue;
        QString device = cols[0];
        QString fstype = cols[1];
        QString mountpoint = cols[6];
        if (!validTypes.contains(fstype)) continue;
        bool ok1, ok2, ok3;
        qint64 total = cols[2].toLongLong(&ok1);
        qint64 used = cols[3].toLongLong(&ok2);
        qint64 free = cols[4].toLongLong(&ok3);
        if (!ok1 || !ok2 || !ok3 || total == 0) continue;
        int percent = (int)(used * 100.0 / total);
        // UI
        QFrame *driveFrame = new QFrame();
        QHBoxLayout *driveLayout = new QHBoxLayout(driveFrame);
        driveLayout->setSpacing(15);
        driveLayout->setContentsMargins(10, 10, 10, 10);
        // Icon
        QLabel *iconLabel = new QLabel();
        QString iconPath = QDir::homePath() + "/.config/Elysia/assets/assets/drive.png";
        if (QFileInfo::exists(iconPath)) {
            QPixmap pixmap(iconPath);
            iconLabel->setPixmap(pixmap.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            iconLabel->setText("Missing");
        }
        iconLabel->setFixedSize(50, 50);
        iconLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        driveLayout->addWidget(iconLabel);
        // Info
        QVBoxLayout *infoContainer = new QVBoxLayout();
        infoContainer->setSpacing(5);
        QLabel *driveLabel = new QLabel(QString("%1 (%2)").arg(device, mountpoint));
        driveLabel->setFont(QFont("ElysiaOSNew", 11));
        driveLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        QLabel *sizeLabel = new QLabel(QString("%1 used / %2 total").arg(bytesToGB(used), bytesToGB(total)));
        sizeLabel->setFont(QFont("ElysiaOSNew", 10));
        sizeLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        QProgressBar *progress = new QProgressBar();
        progress->setMinimum(0);
        progress->setMaximum(100);
        progress->setValue(percent);
        progress->setTextVisible(true);
        progress->setFormat(QString("%1 free").arg(bytesToGB(free)));
        progress->setFixedHeight(20);
        progress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        progress->setStyleSheet(
            "QProgressBar {"
            "    border: 2px solid rgba(255, 255, 255, 0.1);"
            "    border-radius: 10px;"
            "    background-color: rgba(255, 255, 255, 0.05);"
            "    color: white;"
            "    text-align: center;"
            "}"
            "QProgressBar::chunk {"
            "    background-color: #e5a7c6;"
            "    border-radius: 10px;"
            "}"
        );
        infoContainer->addWidget(driveLabel);
        infoContainer->addWidget(sizeLabel);
        infoContainer->addWidget(progress);
        driveLayout->addLayout(infoContainer);
        drivesContainer->addWidget(driveFrame);
        driveWidgets.append(driveFrame);
    }
}

QString Storage::bytesToGB(qint64 bytes) {
    return QString::asprintf("%.1f GB", bytes / (1024.0 * 1024 * 1024));
} 