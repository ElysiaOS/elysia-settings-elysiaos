#include "Sound.h"
#include <QDir>
#include <QFont>
#include <QProcess>
#include <QVBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QVariant>
#include <QMap>
#include <QPair>
#include <QTextEdit>
#include <QScrollArea>

Sound::Sound(QObject *parent) : QObject(parent) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    audioTimer = nullptr;
    micTimer = nullptr;
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &Sound::refreshState);
    initUI();
    refreshTimer->start(1000);
}

void Sound::initUI() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    soundBox = new QWidget(parentWidget);
    soundBox->setGeometry(500, 100, 600, 660);
    soundBox->setStyleSheet("background-color: rgba(255,255,255,0.05); border: 2px solid rgba(255,255,255,0.15); border-radius: 30px;");
    soundBox->setVisible(false);
    QFont font("ElysiaOSNew", 14);
    // Audio Output
    audioLabel = new QLabel("Audio Output", soundBox);
    audioLabel->setFont(font);
    audioLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    audioLabel->move(220, 30);
    audioDropdown = new QComboBox(soundBox);
    audioDropdown->setGeometry(150, 60, 300, 40);
    audioDropdown->setStyleSheet(
        "QComboBox {"
        "background: rgba(255, 255, 255, 0.2);"
        "color: white;"
        "border: 1px solid rgba(255, 255, 255, 0.2);"
        "border-radius: 36px;"
        "padding: 5px;"
        "font-size: 16px;"
        "}"
        "QComboBox QAbstractItemView {"
        "background: rgba(0, 0, 0, 0.2);"
        "border: 1px solid rgba(255, 255, 255, 0.2);"
        "color: black;"
        "padding: 5px;"
        "margin: 0px;"
        "selection-background-color: transparent;"
        "}"
    );
    connect(audioDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Sound::setAudioOutput);
    audioSlider = new QSlider(Qt::Horizontal, soundBox);
    audioSlider->setGeometry(100, 110, 400, 30);
    audioSlider->setMinimum(0);
    audioSlider->setMaximum(100);
    audioSlider->setStyleSheet(
        "QSlider::horizontal { background: transparent; border: none; padding: 0px; margin: 0px; }"
        "QSlider::groove:horizontal { height: 8px; background: rgba(255, 255, 255, 0.1); border-radius: 4px; }"
        "QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e5a7c6, stop:1 #edcee3); border: 1px solid white; width: 20px; margin: -8px 0; border-radius: 10px; }"
        "QSlider::sub-page:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e5a7c6, stop:1 #edcee3); border-radius: 4px; }"
    );
    connect(audioSlider, &QSlider::valueChanged, this, &Sound::debounceAudio);
    // Microphone Input
    micLabel = new QLabel("Microphone Input", soundBox);
    micLabel->setFont(font);
    micLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    micLabel->move(220, 170);
    micDropdown = new QComboBox(soundBox);
    micDropdown->setGeometry(150, 200, 300, 40);
    micDropdown->setStyleSheet(audioDropdown->styleSheet());
    connect(micDropdown, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Sound::setMicInput);
    micSlider = new QSlider(Qt::Horizontal, soundBox);
    micSlider->setGeometry(100, 250, 400, 30);
    micSlider->setMinimum(0);
    micSlider->setMaximum(100);
    micSlider->setStyleSheet(audioSlider->styleSheet());
    connect(micSlider, &QSlider::valueChanged, this, &Sound::debounceMic);
    // Playback Clients
    clientsLabel = new QLabel("Apps Using Audio", soundBox);
    clientsLabel->setFont(font);
    clientsLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    clientsLabel->move(220, 320);
    
    clientsScrollArea = new QScrollArea(soundBox);
    clientsScrollArea->setGeometry(100, 350, 400, 200);
    clientsScrollArea->setWidgetResizable(true);
    clientsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    clientsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    clientsWidget = new QWidget();
    clientsLayout = new QVBoxLayout(clientsWidget);
    clientsLayout->setSpacing(15);
    clientsLayout->setContentsMargins(15, 15, 15, 15);
    clientsLayout->setAlignment(Qt::AlignTop);
    clientsScrollArea->setWidget(clientsWidget);
    // Apply transparent style after both widgets are constructed
    clientsScrollArea->setStyleSheet(
        "QScrollArea, QScrollArea > QWidget { color: white; background: transparent; border: none; margin: 0px; padding: 0px; }"
        "QScrollBar:vertical { background: rgba(255,255,255,0.1); width: 10px; border-radius: 5px; }"
        "QScrollBar::handle:vertical { background: rgba(229,167,198,0.6); border-radius: 5px; min-height: 20px; }"
        "QScrollBar::handle:vertical:hover { background: rgba(229,167,198,0.8); }"
    );
    clientsWidget->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px;");
    // Apply button
    applyBtn = new QPushButton("Apply Changes", soundBox);
    applyBtn->setGeometry(200, 570, 200, 50);
    applyBtn->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e5a7c6, stop:1 #edcee3);"
        "color: white;"
        "font-weight: bold;"
        "border: none;"
        "padding: 6px 12px;"
        "letter-spacing: 1px;"
        "border-radius: 10px;"
        "font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #edcee3, stop:1 #f5e5f0);"
        "}"
        "QPushButton:pressed {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #d495b8, stop:1 #e5a7c6);"
        "}"
    );
    connect(applyBtn, &QPushButton::clicked, this, &Sound::applyChanges);
    updateDropdowns();
    updatePlaybackClients();
}

void Sound::show() {
    updateDropdowns();
    soundBox->setVisible(true);
}

void Sound::hide() {
    soundBox->setVisible(false);
}

QString simplifyDescription(const QString &text) {
    QString t = text;
    t.replace("Analog Stereo", "");
    QRegularExpression re1("Family\\s+[\\w/]+?\\s+HD\\s+Audio\\s+Controller", QRegularExpression::CaseInsensitiveOption);
    t.replace(re1, "");
    QRegularExpression re2("[\\s\\-]+$", QRegularExpression::CaseInsensitiveOption);
    t.replace(re2, "");
    return t.trimmed();
}

// Helper: get all sinks/sources with description, ports, and active port
struct AudioDevice {
    QString name;
    QString description;
    QList<QPair<QString, QString>> ports; // (port, port description)
    QString activePort;
};

QList<AudioDevice> getAudioDevices(const QString &type) {
    QList<AudioDevice> devices;
    QProcess shortProc, fullProc;
    shortProc.start("pactl", QStringList() << "list" << "short" << (type + "s"));
    shortProc.waitForFinished();
    QString shortOut = QString::fromUtf8(shortProc.readAllStandardOutput());
    fullProc.start("pactl", QStringList() << "list" << (type + "s"));
    fullProc.waitForFinished();
    QString fullOut = QString::fromUtf8(fullProc.readAllStandardOutput());
    QString blockHeader = type[0].toUpper() + type.mid(1) + " #";
    QStringList blocks = fullOut.split(blockHeader, Qt::SkipEmptyParts);
    QMap<QString, QString> nameToDesc;
    QMap<QString, QList<QPair<QString, QString>>> nameToPorts;
    QMap<QString, QString> nameToActivePort;
    for (const QString &block : blocks) {
        QString name, desc, activePort;
        QList<QPair<QString, QString>> ports;
        QStringList lines = block.split('\n');
        for (int i = 0; i < lines.size(); ++i) {
            QString line = lines[i].trimmed();
            if (line.startsWith("Name:")) {
                name = line.section(":", 1).trimmed();
            } else if (line.startsWith("Description:")) {
                desc = line.section(":", 1).trimmed();
            } else if (line.startsWith("Ports:")) {
                // Parse ports until next empty line or non-indented line
                int j = i + 1;
                while (j < lines.size() && (lines[j].startsWith(" ") || lines[j].startsWith("\t"))) {
                    QString portLine = lines[j].trimmed();
                    if (portLine.contains(":")) {
                        QString portName = portLine.section(":", 0, 0).trimmed();
                        QString portDesc = portLine.section(":", 1).section("(", 0, 0).trimmed();
                        ports.append(qMakePair(portName, portDesc));
                    }
                    ++j;
                }
            } else if (line.startsWith("Active Port:")) {
                activePort = line.section(":", 1).trimmed();
            }
        }
        if (!name.isEmpty()) {
            QString cleanDesc = desc.isEmpty() ? name : simplifyDescription(desc);
            nameToDesc[name] = cleanDesc;
            nameToPorts[name] = ports;
            nameToActivePort[name] = activePort;
        }
    }
    QStringList shortLines = shortOut.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : shortLines) {
        QStringList parts = line.split('\t');
        if (parts.size() < 2) continue;
        QString devName = parts[1];
        QString desc = nameToDesc.value(devName, devName);
        QList<QPair<QString, QString>> ports = nameToPorts.value(devName);
        QString activePort = nameToActivePort.value(devName);
        devices.append({devName, desc, ports, activePort});
    }
    return devices;
}

void Sound::updateDropdowns() {
    // Audio Output
    audioDropdown->blockSignals(true);
    audioDropdown->clear();
    QList<AudioDevice> sinks = getAudioDevices("sink");
    int audioActiveIdx = 0, audioIdx = 0;
    for (const auto &dev : sinks) {
        for (const auto &port : dev.ports) {
            QString label = dev.description + " - " + port.second;
            audioDropdown->addItem(label, QVariant::fromValue(QPair<QString,QString>(dev.name, port.first)));
            if (dev.activePort == port.first) audioActiveIdx = audioIdx;
            ++audioIdx;
        }
    }
    audioDropdown->setCurrentIndex(audioActiveIdx);
    audioDropdown->blockSignals(false);
    // Mic Input
    micDropdown->blockSignals(true);
    micDropdown->clear();
    QList<AudioDevice> sources = getAudioDevices("source");
    int micActiveIdx = 0, micIdx = 0;
    for (const auto &dev : sources) {
        if (dev.name.contains(".monitor")) continue;
        for (const auto &port : dev.ports) {
            QString label = dev.description + " - " + port.second;
            micDropdown->addItem(label, QVariant::fromValue(QPair<QString,QString>(dev.name, port.first)));
            if (dev.activePort == port.first) micActiveIdx = micIdx;
            ++micIdx;
        }
    }
    micDropdown->setCurrentIndex(micActiveIdx);
    micDropdown->blockSignals(false);
}

QPair<QString,QString> getDropdownDevicePort(QComboBox *dropdown) {
    QVariant v = dropdown->currentData();
    if (v.canConvert<QPair<QString,QString>>()) {
        return v.value<QPair<QString,QString>>();
    }
    return QPair<QString,QString>();
}

void Sound::setAudioOutput() {
    auto pair = getDropdownDevicePort(audioDropdown);
    if (!pair.first.isEmpty() && !pair.second.isEmpty()) {
        QProcess::execute("pactl", QStringList() << "set-default-sink" << pair.first);
        QProcess::execute("pactl", QStringList() << "set-sink-port" << pair.first << pair.second);
    }
}

void Sound::setMicInput() {
    auto pair = getDropdownDevicePort(micDropdown);
    if (!pair.first.isEmpty() && !pair.second.isEmpty()) {
        QProcess::execute("pactl", QStringList() << "set-default-source" << pair.first);
        QProcess::execute("pactl", QStringList() << "set-source-port" << pair.first << pair.second);
    }
}

void Sound::setAudioVolume(int value) {
    auto pair = getDropdownDevicePort(audioDropdown);
    if (!pair.first.isEmpty()) {
        QProcess::execute("pactl", QStringList() << "set-sink-volume" << pair.first << QString::number(value) + "%");
    }
}

void Sound::setMicVolume(int value) {
    auto pair = getDropdownDevicePort(micDropdown);
    if (!pair.first.isEmpty()) {
        QProcess::execute("pactl", QStringList() << "set-source-volume" << pair.first << QString::number(value) + "%");
    }
}

void Sound::debounceAudio(int value) {
    if (audioTimer) audioTimer->stop();
    if (!audioTimer) audioTimer = new QTimer(this);
    audioTimer->setSingleShot(true);
    connect(audioTimer, &QTimer::timeout, this, [=]() { setAudioVolume(value); });
    audioTimer->start(100);
}

void Sound::debounceMic(int value) {
    if (micTimer) micTimer->stop();
    if (!micTimer) micTimer = new QTimer(this);
    micTimer->setSingleShot(true);
    connect(micTimer, &QTimer::timeout, this, [=]() { setMicVolume(value); });
    micTimer->start(100);
}

void Sound::applyChanges() {
    setAudioOutput();
    setMicInput();
    setAudioVolume(audioSlider->value());
    setMicVolume(micSlider->value());
    QMessageBox::information(soundBox, "Audio", "Settings applied.");
}

void Sound::refreshState() {
    updateSlider(audioDropdown, audioSlider, true);
    updateSlider(micDropdown, micSlider, false);
    updatePlaybackClients();
}

void Sound::updateSlider(QComboBox *dropdown, QSlider *slider, bool isSink) {
    auto pair = getDropdownDevicePort(dropdown);
    if (pair.first.isEmpty()) return;
    QString kind = isSink ? "sink" : "source";
    QProcess process;
    process.start("pactl", QStringList() << "get-" + kind + "-volume" << pair.first);
    process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    QRegularExpression re("(\\d+%)");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch()) {
        QString percentStr = match.captured(0);
        int percent = percentStr.remove("%").toInt();
        if (slider->value() != percent) {
            slider->blockSignals(true);
            slider->setValue(percent);
            slider->blockSignals(false);
        }
    }
}

QList<QMap<QString, QString>> Sound::getPlaybackClients() {
    QProcess process;
    process.start("pactl", QStringList() << "list" << "sink-inputs");
    process.waitForFinished();
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    
    QList<QMap<QString, QString>> clients;
    QMap<QString, QString> current;
    
    QStringList lines = output.split('\n');
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.startsWith("Sink Input #")) {
            if (!current.isEmpty()) {
                clients.append(current);
            }
            current.clear();
            QString index = trimmedLine.split("#")[1];
            current["index"] = index;
        } else if (trimmedLine.startsWith("application.name")) {
            QRegularExpression re("\"([^\"]+)\"");
            QRegularExpressionMatch match = re.match(trimmedLine);
            if (match.hasMatch()) {
                current["name"] = match.captured(1);
            }
        } else if (trimmedLine.startsWith("Volume:")) {
            QRegularExpression re("(\\d+)%");
            QRegularExpressionMatchIterator it = re.globalMatch(trimmedLine);
            QList<int> volumes;
            while (it.hasNext()) {
                QRegularExpressionMatch match = it.next();
                volumes.append(match.captured(1).toInt());
            }
            if (!volumes.isEmpty()) {
                int avgVolume = 0;
                for (int vol : volumes) {
                    avgVolume += vol;
                }
                avgVolume /= volumes.size();
                current["volume"] = QString::number(avgVolume);
            }
        }
    }
    
    if (!current.isEmpty()) {
        clients.append(current);
    }
    
    return clients;
}

void Sound::updatePlaybackClients() {
    QList<QMap<QString, QString>> clients = getPlaybackClients();
    
    // Clear existing sliders and timers
    QLayoutItem *child;
    while ((child = clientsLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    
    // Clear app timers
    for (auto timer : appTimers.values()) {
        if (timer) timer->stop();
    }
    appTimers.clear();
    appSliders.clear();

    if (clients.isEmpty()) {
        QLabel *noAppsLabel = new QLabel("No applications are currently using audio.", clientsWidget);
        noAppsLabel->setAlignment(Qt::AlignCenter);
        noAppsLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        clientsLayout->addWidget(noAppsLabel);
    } else {
        for (const auto &client : clients) {
            QString name = client.value("name", "Unknown");
            QString volume = client.value("volume", "0");
            QString index = client.value("index", "");
            
            QWidget *appWidget = new QWidget();
            appWidget->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
            QVBoxLayout *appLayout = new QVBoxLayout(appWidget);
            appLayout->setSpacing(8);
            appLayout->setContentsMargins(0, 0, 0, 0);

            QLabel *appNameLabel = new QLabel(name);
            appNameLabel->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px; font-weight: bold;");
            appLayout->addWidget(appNameLabel);

            QSlider *appVolumeSlider = new QSlider(Qt::Horizontal, appWidget);
            appVolumeSlider->setMinimum(0);
            appVolumeSlider->setMaximum(100);
            appVolumeSlider->setValue(volume.toInt());
            appVolumeSlider->setStyleSheet(
                "QSlider::horizontal { background: transparent; border: none; padding: 0px; margin: 0px; }"
                "QSlider::groove:horizontal { height: 6px; background: rgba(255, 255, 255, 0.1); border-radius: 3px; }"
                "QSlider::handle:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e5a7c6, stop:1 #edcee3); border: 1px solid white; width: 16px; margin: -6px 0; border-radius: 8px; }"
                "QSlider::sub-page:horizontal { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #e5a7c6, stop:1 #edcee3); border-radius: 3px; }"
            );
            
            // Create debouncing timer for this app
            QTimer *appTimer = new QTimer(this);
            appTimer->setSingleShot(true);
            appTimer->setInterval(150); // Shorter debounce for better responsiveness
            appTimers[index] = appTimer;
            appSliders[index] = appVolumeSlider;
            
            connect(appVolumeSlider, &QSlider::valueChanged, this, [this, index, appVolumeSlider, appTimer]() {
                appTimer->stop();
                appTimer->disconnect();
                connect(appTimer, &QTimer::timeout, this, [this, index, appVolumeSlider]() {
                    QProcess::execute("pactl", QStringList() << "set-sink-input-volume" << index << QString::number(appVolumeSlider->value()) + "%");
                });
                appTimer->start();
            });
            
            appLayout->addWidget(appVolumeSlider);
            clientsLayout->addWidget(appWidget);
        }
    }
}

void Sound::onAppVolumeChanged(int value) {
    // This function is called when an app's volume slider changes
    // The actual volume change is handled in the lambda function above
    Q_UNUSED(value)
} 