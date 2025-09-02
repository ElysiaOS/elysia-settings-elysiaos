#include "Battery.h"
#include "../translations.h"
#include <QDir>
#include <QFont>
#include <QProcess>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>

Battery::Battery(QObject *parent) : QObject(parent) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    batteryDevice = findBatteryDevice();
    initBatteryUI();
    updateTimer = new QTimer(this);
    updateTimer->setInterval(1000);
    connect(updateTimer, &QTimer::timeout, this, &Battery::updateBatteryStatus);
}

QString Battery::findBatteryDevice() {
    QProcess process;
    process.start("upower", QStringList() << "-e");
    process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        if (line.contains("battery_")) return line.trimmed();
    }
    return QString();
}

QMap<QString, QString> Battery::parseBatteryInfo(const QString &device) {
    QMap<QString, QString> info;
    QProcess process;
    process.start("upower", QStringList() << "-i" << device);
    process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    QStringList wantedKeys = {"state", "warning-level", "energy-full", "energy-full-design", "energy-rate", "voltage", "charge-cycles", "percentage", "capacity", "technology"};
    for (const QString &line : output.split('\n')) {
        if (!line.contains(":")) continue;
        QStringList parts = line.trimmed().split(":", Qt::SkipEmptyParts);
        if (parts.size() < 2) continue;
        QString key = parts[0].trimmed();
        QString value = parts[1].trimmed();
        if (wantedKeys.contains(key)) info[key] = value;
    }
    return info;
}

void Battery::initBatteryUI() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    batteryBox = new QWidget(parentWidget);
    batteryBox->setGeometry(500, 100, 600, 500);
    batteryBox->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.05);"
        "border: 2px solid rgba(255, 255, 255, 0.15);"
        "border-radius: 30px;"
    );
    batteryBox->setVisible(false);
    QVBoxLayout *layout = new QVBoxLayout(batteryBox);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(18);
    QLabel *title = new QLabel(Translations::tr("BATTERY_TITLE"), batteryBox);
    title->setFont(QFont("ElysiaOSNew", 17, QFont::Bold));
    title->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);
    if (batteryDevice.isEmpty()) {
        QLabel *label = new QLabel(Translations::tr("BATTERY_TITLE"), batteryBox);
        label->setFont(QFont("ElysiaOSNew", 12));
        label->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        label->setWordWrap(true);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
        infoLabels.clear();
        infoLabels.append(label);
        return;
    }
    QStringList keys = {"state", "warning-level", "energy-full", "energy-full-design", "energy-rate", "voltage", "charge-cycles", "percentage", "capacity", "technology"};
    infoLabels.clear();
    for (const QString &key : keys) {
        QLabel *label = new QLabel("", batteryBox);
        label->setFont(QFont("ElysiaOSNew", 12));
        label->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        label->setWordWrap(true);
        layout->addWidget(label);
        infoLabels.append(label);
    }
    updateBatteryStatus();
}

void Battery::show() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    if (parentWidget->metaObject()->indexOfMethod("hideTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideTiles");
    if (parentWidget->metaObject()->indexOfMethod("showBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showBackButton");
    batteryBox->setVisible(true);
    updateBatteryStatus();
    updateTimer->start();
}

void Battery::hide() {
    batteryBox->setVisible(false);
    updateTimer->stop();
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    if (parentWidget->metaObject()->indexOfMethod("hideBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideBackButton");
    if (parentWidget->metaObject()->indexOfMethod("showTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showTiles");
}

void Battery::updateBatteryStatus() {
    if (batteryDevice.isEmpty()) return;
    QMap<QString, QString> info = parseBatteryInfo(batteryDevice);
    QStringList keys = {"state", "warning-level", "energy-full", "energy-full-design", "energy-rate", "voltage", "charge-cycles", "percentage", "capacity", "technology"};
    for (int i = 0; i < keys.size() && i < infoLabels.size(); ++i) {
        QString value = info.value(keys[i], "N/A");
        infoLabels[i]->setText(QString("%1: %2").arg(keys[i].replace("-", " ").toUpper(), value));
    }
} 