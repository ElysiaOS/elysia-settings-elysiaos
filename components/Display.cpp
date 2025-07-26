#include "Display.h"
#include <QDir>
#include <QFont>
#include <QPixmap>
#include <QProcess>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

DisplayManager::DisplayManager(QObject *parent) : QObject(parent) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    
    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(5000);
    connect(refreshTimer, &QTimer::timeout, this, &DisplayManager::autoRefresh);
    
    // Desktop label
    desktopLabel = new QLabel("", parentWidget);
    QFont desktopFont("ElysiaOSNew", 16, QFont::Bold);
    desktopLabel->setFont(desktopFont);
    desktopLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    desktopLabel->move(760, 0);
    desktopLabel->setVisible(false);
    
    // Desktop image label
    desktopImageLabel = new QLabel(parentWidget);
    desktopImageLabel->setGeometry(650, 45, 300, 335);
    desktopImageLabel->setAlignment(Qt::AlignCenter);
    desktopImageLabel->setVisible(false);
    
    // Display box
    displayBox = new QWidget(parentWidget);
    displayBox->setGeometry(500, 100, 600, 500);
    displayBox->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.05);"
        "border: 2px solid rgba(255, 255, 255, 0.15);"
        "border-radius: 30px;"
    );
    displayBox->setVisible(false);
    
    // Resolution label
    resolutionLabel = new QLabel("Resolution", displayBox);
    QFont resFont("ElysiaOSNew", 14);
    resolutionLabel->setFont(resFont);
    resolutionLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    resolutionLabel->move(220, 205);
    
    // Display dropdown
    displayDropdown = new QComboBox(displayBox);
    displayDropdown->setGeometry(150, 240, 300, 40);
    displayDropdown->setStyleSheet(
        "QComboBox {"
        "    background: rgba(255, 255, 255, 0.2);"
        "    color: white;"
        "    border: 1px solid rgba(255, 255, 255, 0.25);"
        "    border-radius: 21px;"
        "    padding: 5px;"
        "    font-size: 16px;"
        "}"
        "QComboBox:hover {"
        "    background: rgba(255, 255, 255, 0.3);"
        "}"
        "QComboBox QAbstractItemView {"
        "    background: rgba(0, 0, 0, 0.2);"
        "    border: none;"
        "    color: white;"
        "    border-radius: 21px;"
        "    padding: 5px;"
        "    selection-background-color: rgba(255, 255, 255, 0.2);"
        "}"
    );
    
    // Brightness label
    brightnessLabel = new QLabel("Brightness", displayBox);
    QFont brightFont("ElysiaOSNew", 14);
    brightnessLabel->setFont(brightFont);
    brightnessLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    brightnessLabel->move(220, 300);
    
    // Brightness slider
    brightnessSlider = new QSlider(Qt::Horizontal, displayBox);
    brightnessSlider->setGeometry(100, 340, 400, 30);
    brightnessSlider->setMinimum(1);
    brightnessSlider->setMaximum(100);
    brightnessSlider->setStyleSheet(
        "QSlider::horizontal {"
        "    background: transparent;"
        "    border: none;"
        "    padding: 0px;"
        "    margin: 0px;"
        "}"
        "QSlider::groove:horizontal {"
        "    height: 8px;"
        "    background: transparent;"
        "    border: none;"
        "    padding: 0px;"
        "    margin: 0px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: pink;"
        "    border: 1px solid white;"
        "    width: 18px;"
        "    margin: -6px 0;"
        "    border-radius: 9px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "    background: rgba(255, 255, 255, 0.25);"
        "    border-radius: 5px;"
        "}"
        "QSlider::add-page:horizontal {"
        "    background: rgba(255, 255, 255, 0.05);"
        "    border-radius: 5px;"
        "}"
    );
    connect(brightnessSlider, &QSlider::valueChanged, this, &DisplayManager::setBrightness);
    
    // Apply button
    applyBtn = new QPushButton("Apply Changes", displayBox);
    applyBtn->setGeometry(200, 400, 200, 50);
    applyBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #e5a7c6;"
        "    color: white;"
        "    font-weight: bold;"
        "    border: none;"
        "    padding: 6px 12px;"
        "    letter-spacing: 1px;"
        "    border-radius: 10px;"
        "    font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #edcee3;"
        "}"
    );
    connect(applyBtn, &QPushButton::clicked, this, &DisplayManager::applyDisplayChange);
}

QString DisplayManager::detectThemeImage() {
    QString basePath = QDir::homePath() + "/.config/Elysia/assets/";
    try {
        QProcess process;
        process.start("gsettings", QStringList() << "get" << "org.gnome.desktop.interface" << "color-scheme");
        if (process.waitForFinished()) {
            QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
            if (output.contains("prefer-light")) {
                return basePath + "assets/light1.png";
            } else {
                return basePath + "assets/dark1.png";
            }
        }
    } catch (...) {
        // Fall through to default
    }
    return basePath + "assets/light1.png";
}

QStringList DisplayManager::getMonitorModes() {
    QStringList modes;
    try {
        QProcess process;
        process.start("hyprctl", QStringList() << "monitors");
        if (process.waitForFinished()) {
            QString output = QString::fromUtf8(process.readAllStandardOutput());
            QRegularExpression re(R"(\d{3,4}x\d{3,4}@\d{2,3})");
            QRegularExpressionMatchIterator it = re.globalMatch(output);
            while (it.hasNext()) {
                QString mode = it.next().captured(0);
                if (!modes.contains(mode)) {
                    modes.append(mode);
                }
            }
        }
    } catch (...) {
        // Return empty list on error
    }
    std::sort(modes.begin(), modes.end());
    return modes;
}

void DisplayManager::updateDisplayDropdown() {
    QStringList modes = getMonitorModes();
    QStringList currentItems;
    for (int i = 0; i < displayDropdown->count(); ++i) {
        currentItems.append(displayDropdown->itemText(i));
    }
    
    if (modes != currentItems) {
        displayDropdown->clear();
        displayDropdown->addItems(modes);
    }
    
    QString currentMode;
    QString configPath = QDir::homePath() + "/.config/hypr/hyprland.conf";
    QFile file(configPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("monitor") && line.contains("@")) {
                QRegularExpression re(R"(\d{3,4}x\d{3,4}@\d{2,3})");
                QRegularExpressionMatch match = re.match(line);
                if (match.hasMatch()) {
                    currentMode = match.captured(0);
                }
                QStringList parts = line.split(QRegularExpression("\\s+"));
                if (parts.size() > 1) {
                    monitorName = parts[1];
                }
                break;
            }
        }
        file.close();
    }
    
    if (!currentMode.isEmpty() && modes.contains(currentMode)) {
        displayDropdown->setCurrentText(currentMode);
    }
}

void DisplayManager::updateThemeImage() {
    QString imagePath = detectThemeImage();
    QPixmap pixmap(imagePath);
    desktopImageLabel->setPixmap(pixmap.scaled(300, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void DisplayManager::setBrightness(int value) {
    try {
        QProcess::startDetached("brightnessctl", QStringList() << "set" << QString::number(value) + "%");
    } catch (...) {
        QMessageBox::warning(qobject_cast<QWidget*>(parent()), "Error", "Failed to set brightness");
    }
}

void DisplayManager::updateBrightnessSlider() {
    try {
        QProcess process;
        process.start("brightnessctl", QStringList() << "g");
        if (process.waitForFinished()) {
            QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
            
            QProcess maxProcess;
            maxProcess.start("brightnessctl", QStringList() << "m");
            if (maxProcess.waitForFinished()) {
                QString maxOutput = QString::fromUtf8(maxProcess.readAllStandardOutput()).trimmed();
                
                bool ok1, ok2;
                int current = output.toInt(&ok1);
                int max = maxOutput.toInt(&ok2);
                
                if (ok1 && ok2 && max > 0) {
                    int percent = (current * 100) / max;
                    if (brightnessSlider->value() != percent) {
                        brightnessSlider->setValue(percent);
                    }
                }
            }
        }
    } catch (...) {
        brightnessSlider->setValue(50);
    }
}

QPair<int, QStringList> DisplayManager::getCurrentMonitorLine() {
    QString path = QDir::homePath() + "/.config/hypr/hyprland.conf";
    QFile file(path);
    QStringList lines;
    
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            lines.append(in.readLine());
        }
        file.close();
    }
    
    for (int i = 0; i < lines.size(); ++i) {
        if (lines[i].startsWith("monitor") && lines[i].contains("@")) {
            return qMakePair(i, lines);
        }
    }
    
    return qMakePair(-1, lines);
}

void DisplayManager::applyDisplayChange() {
    QString newMode = displayDropdown->currentText();
    QPair<int, QStringList> result = getCurrentMonitorLine();
    int index = result.first;
    QStringList lines = result.second;
    
    if (index == -1) {
        QMessageBox::warning(qobject_cast<QWidget*>(parent()), "Error", "Could not find monitor config line.");
        return;
    }
    
    QRegularExpression re(R"(\d{3,4}x\d{3,4}@\d{2,3})");
    lines[index] = lines[index].replace(re, newMode);
    
    QString configPath = QDir::homePath() + "/.config/hypr/hyprland.conf";
    QFile file(configPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &line : lines) {
            out << line << "\n";
        }
        file.close();
    } else {
        QMessageBox::warning(qobject_cast<QWidget*>(parent()), "Error", "Failed to write file");
        return;
    }
    
    QMessageBox::information(qobject_cast<QWidget*>(parent()), "Success", 
                           QString("Display mode changed to %1.\nRestart Hyprland for changes to apply.").arg(newMode));
}

void DisplayManager::show() {
    updateDisplayDropdown();
    updateBrightnessSlider();
    updateThemeImage();
    
    desktopImageLabel->setVisible(true);
    desktopLabel->setVisible(true);
    displayBox->setVisible(true);
    
    refreshTimer->start();
}

void DisplayManager::hide() {
    refreshTimer->stop();
    displayBox->setVisible(false);
    desktopImageLabel->setVisible(false);
    desktopLabel->setVisible(false);
}

void DisplayManager::autoRefresh() {
    if (displayBox->isVisible()) {
        updateBrightnessSlider();
        updateDisplayDropdown();
        updateThemeImage();
    }
} 