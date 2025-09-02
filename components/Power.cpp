#include "Power.h"
#include "../translations.h"
#include <QDir>
#include <QFont>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QStackedLayout>
#include <QMap>
#include <QTimer>
#include <QFileInfo>
#include <QPixmap>
#include <QMessageBox>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

Power::Power(QObject *parent) : QObject(parent) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    initUI();
}

void Power::initUI() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    powerBox = new QWidget(parentWidget);
    powerBox->setGeometry(455, 100, 700, 470);
    powerBox->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.05);"
        "border: 2px solid rgba(255, 255, 255, 0.15);"
        "border-radius: 30px;"
    );
    powerBox->setVisible(false);
    QVBoxLayout *layout = new QVBoxLayout(powerBox);
    layout->setContentsMargins(150, 100, 150, 200); // match Python
    layout->setSpacing(25);
    QLabel *powerLabel = new QLabel(Translations::tr("POWER_OPTIONS"), powerBox);
    powerLabel->setFont(QFont("ElysiaOSNew", 16));
    powerLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    powerLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(powerLabel);
    // Mode row as layout
    QHBoxLayout *modeContainer = new QHBoxLayout();
    modeContainer->setSpacing(30);
    modeContainer->setAlignment(Qt::AlignCenter);
    QMap<QString, QPair<QString, QString>> modes = {
        {"performance", {Translations::tr("PERFORMANCE"), "performance.png"}},
        {"balanced", {Translations::tr("BALANCED"), "balanced.png"}},
        {"default", {Translations::tr("DEFAULT"), "default.png"}}
    };
    for (auto it = modes.begin(); it != modes.end(); ++it) {
        QString modeKey = it.key();
        QString labelText = it.value().first;
        QString imageFile = it.value().second;
        QVBoxLayout *outerLayout = new QVBoxLayout();
        outerLayout->setAlignment(Qt::AlignCenter);
        outerLayout->setSpacing(8);
        // Label above icon
        QLabel *label = new QLabel(labelText);
        label->setFont(QFont("ElysiaOSNew", 12));
        label->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        label->setAlignment(Qt::AlignCenter);
        outerLayout->addWidget(label, 0, Qt::AlignCenter);
        // Mode tile frame
        QFrame *frame = new QFrame(powerBox);
        frame->setFixedSize(130, 130);
        frame->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        frame->setCursor(Qt::PointingHandCursor);
        QStackedLayout *stackedLayout = new QStackedLayout(frame);
        QLabel *imageLabel = new QLabel();
        QString iconPath = QDir::homePath() + "/.config/Elysia/assets/assets/" + imageFile;
        if (QFileInfo::exists(iconPath)) {
            QPixmap pixmap(iconPath);
            imageLabel->setPixmap(pixmap.scaled(130, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            imageLabel->setText(Translations::tr("MISSING"));
        }
        QLabel *tick = new QLabel(QString::fromUtf8("\u2714"));
        tick->setStyleSheet("color: #e5a7c6; font-size: 24px; background: transparent; border: none;");
        tick->setAlignment(Qt::AlignTop | Qt::AlignRight);
        tick->setVisible(false);
        stackedLayout->addWidget(imageLabel);
        stackedLayout->addWidget(tick);
        frame->installEventFilter(this);
        frame->setProperty("modeKey", modeKey);
        modeBoxes[modeKey] = qMakePair(frame, tick);
        outerLayout->addWidget(frame, 0, Qt::AlignCenter);
        modeContainer->addLayout(outerLayout);
    }
    layout->addLayout(modeContainer);
    // Apply button
    applyBtn = new QPushButton(Translations::tr("APPLY_CHANGES"), powerBox);
    applyBtn->setFixedSize(170, 40);
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
    applyBtn->setToolTip(Translations::tr("APPLY_TOOLTIP"));
    connect(applyBtn, &QPushButton::clicked, this, &Power::applyChanges);
    // Place button near the bottom center of the box
    applyBtn->move((680-140)/2, 400);
}

void Power::show() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (parentWidget && parentWidget->metaObject()->indexOfMethod("hideTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideTiles");
    if (parentWidget && parentWidget->metaObject()->indexOfMethod("showBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showBackButton");
    powerBox->setVisible(true);
    // Run detection in background
    if (!modeDetectWatcher) {
        modeDetectWatcher = new QFutureWatcher<QString>(this);
        connect(modeDetectWatcher, &QFutureWatcher<QString>::finished, this, &Power::onModeDetectFinished);
    }
    QFuture<QString> future = QtConcurrent::run([=]() {
        QProcess process;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("DISPLAY", qgetenv("DISPLAY"));
        env.insert("XAUTHORITY", qgetenv("XAUTHORITY"));
        env.insert("DBUS_SESSION_BUS_ADDRESS", qgetenv("DBUS_SESSION_BUS_ADDRESS"));
        process.setProcessEnvironment(env);
        process.start("pkexec", QStringList() << "auto-cpufreq" << "--debug");
        if (!process.waitForStarted(5000)) return QString();
        process.waitForFinished(15000);
        QString output = QString::fromUtf8(process.readAllStandardOutput());
        if (output.contains("Currently using: performance governor")) return QString("performance");
        if (output.contains("Currently using: powersave governor")) return QString("balanced");
        if (output.contains("Currently using: reset governor")) return QString("default");
        return QString();
    });
    modeDetectWatcher->setFuture(future);
}

void Power::onModeDetectFinished() {
    if (!modeDetectWatcher) return;
    QString mode = modeDetectWatcher->result();
    if (!mode.isEmpty()) selectMode(mode);
}

void Power::hide() {
    powerBox->setVisible(false);
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (parentWidget && parentWidget->metaObject()->indexOfMethod("hideBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideBackButton");
    if (parentWidget && parentWidget->metaObject()->indexOfMethod("showTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showTiles");
}

void Power::selectMode(const QString &mode) {
    selectedMode = mode;
    for (auto it = modeBoxes.begin(); it != modeBoxes.end(); ++it) {
        bool isSelected = (it.key() == mode);
        it.value().first->setStyleSheet(QString("border: %1 solid #e5a7c6; border-radius: 26px; background: transparent;").arg(isSelected ? "3px" : "2px transparent"));
        it.value().second->setVisible(isSelected);
    }
}

void Power::applyChanges() {
    if (selectedMode.isEmpty()) return;
    QMap<QString, QStringList> commands = {
        {"performance", {"auto-cpufreq", "--force=performance"}},
        {"balanced", {"auto-cpufreq", "--force=powersave"}},
        {"default", {"auto-cpufreq", "--force=reset"}}
    };
    QStringList args = commands.value(selectedMode);
    if (!args.isEmpty()) {
        QProcess process;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("DISPLAY", qgetenv("DISPLAY"));
        env.insert("XAUTHORITY", qgetenv("XAUTHORITY"));
        env.insert("DBUS_SESSION_BUS_ADDRESS", qgetenv("DBUS_SESSION_BUS_ADDRESS"));
        process.setProcessEnvironment(env);
        process.start("pkexec", args);
        if (!process.waitForStarted(5000)) {
            QMessageBox::warning(powerBox, Translations::tr("ERROR_FASTFETCH"), "Failed to start pkexec.\nCheck your polkit setup and environment.");
            return;
        }
        process.waitForFinished(-1); // Wait for user to complete polkit prompt
        if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
            QMessageBox::warning(powerBox, Translations::tr("ERROR_FASTFETCH"), "Failed to apply CPU profile.\n\nOutput:\n" + process.readAllStandardError());
        }
    }
}

void Power::autoDetectCurrentMode() {
    QProcess process;
    process.start("pkexec", QStringList() << "auto-cpufreq" << "--debug");
    process.waitForFinished(15000);
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    if (output.contains("Currently using: performance governor")) {
        selectMode("performance");
    } else if (output.contains("Currently using: powersave governor")) {
        selectMode("balanced");
    } else if (output.contains("Currently using: reset governor")) {
        selectMode("default");
    }
}

// Event filter for mode frame clicks
#include <QEvent>
bool Power::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        for (auto it = modeBoxes.begin(); it != modeBoxes.end(); ++it) {
            if (it.value().first == obj) {
                selectMode(it.key());
                break;
            }
        }
    }
    return QObject::eventFilter(obj, event);
} 