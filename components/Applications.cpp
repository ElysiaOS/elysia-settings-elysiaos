#include "Applications.h"
#include "../translations.h"
#include <QDir>
#include <QFont>
#include <QProcess>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QCheckBox>
#include <QFileSystemWatcher>

Applications::Applications(QObject *parent) : QObject(parent), currentConfigIdx(0) {
    configFiles = { 
        QDir::homePath() + "/.config/hypr/applications.conf",
        QDir::homePath() + "/.config/hypr/app_keybinds.conf",
        QDir::homePath() + "/.config/hypr/auto_start.conf",
        QDir::homePath() + "/.config/hypr/window_rules.conf"
    };
    configNames = { Translations::tr("APPLICATIONS"), Translations::tr("KEYBINDS"), Translations::tr("AUTORUNS"), Translations::tr("WINDOW_RULES") };
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    initUI();
    fileWatcher = new QFileSystemWatcher(this);
    fileWatcher->addPath(configFiles[0]);
    fileWatcher->addPath(configFiles[1]);
    fileWatcher->addPath(configFiles[2]);
    fileWatcher->addPath(configFiles[3]);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path) {
        loadConfig();
        fileWatcher->addPath(path); // Re-add in case it was removed after change
    });
}

void Applications::initUI() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (!parentWidget) return;
    container = new QWidget(parentWidget);
    container->setGeometry(80, 80, 1440, 640);
    container->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.1);"
        "border: 36px;"
        "border-radius: 36px;"
        "margin: 4px;"
    );
    container->setVisible(false);
    mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(50, 50, 50, 50);
    mainLayout->setSpacing(40);
    // Header with arrows and config name
    headerWidget = new QWidget(container);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    leftArrow = new QPushButton("<", headerWidget);
    rightArrow = new QPushButton(">", headerWidget);
    leftArrow->setFixedSize(50, 50);
    rightArrow->setFixedSize(50, 50);
    leftArrow->setStyleSheet("QPushButton { color: rgba(255,255,255,0.8); background: transparent; border: none; font-size: 24px; font-weight: bold; } QPushButton:hover { color: #e5a7c6; }");
    rightArrow->setStyleSheet("QPushButton { color: rgba(255,255,255,0.8); background: transparent; border: none; font-size: 24px; font-weight: bold; } QPushButton:hover { color: #e5a7c6; }");
    configLabel = new QLabel(configNames[currentConfigIdx], headerWidget);
    configLabel->setFont(QFont("ElysiaOSNew", 20, QFont::Bold));
    configLabel->setStyleSheet("color: rgba(255,255,255,0.95); background: transparent; border: none; border-radius: 0px;");
    configLabel->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(leftArrow);
    headerLayout->addStretch();
    headerLayout->addWidget(configLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(rightArrow);
    mainLayout->addWidget(headerWidget);
    connect(leftArrow, &QPushButton::clicked, this, &Applications::handleLeftArrow);
    connect(rightArrow, &QPushButton::clicked, this, &Applications::handleRightArrow);
    // Scroll area for lines
    QScrollArea *scrollArea = new QScrollArea(container);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; } QScrollBar:vertical { background: transparent; border-radius: 6px; width: 12px; } QScrollBar::handle:vertical { background: rgba(229,167,198,0.5); border-radius: 6px; } QScrollBar::handle:vertical:hover { background: rgba(229,167,198,0.8); }");
    linesWidget = new QWidget(scrollArea);
    linesWidget->setStyleSheet(
        "background-color: transparent;"
        "border: 0px;"
        "border-radius: 36px;"
        "margin: 0px;"
    );
    linesLayout = new QVBoxLayout(linesWidget);
    linesLayout->setContentsMargins(20, 20, 20, 20);
    linesLayout->setSpacing(20);
    scrollArea->setWidget(linesWidget);
    mainLayout->addWidget(scrollArea);
    loadConfig();
}

void Applications::loadConfig() {
    QStringList allLines;
    QFile file(configFiles[currentConfigIdx]);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) allLines.append(in.readLine());
        file.close();
    }
    // Separate header based on current config type
    int firstConfigIdx = 0;
    if (currentConfigIdx == 0) {
        // Applications config - ignore ########, ##APPS##, ########
        for (int i = 0; i < allLines.size(); ++i) {
            QString line = allLines[i].trimmed();
            if (line.startsWith("########") || line.startsWith("##APPS##")) {
                firstConfigIdx = i + 1;
            }
        }
    } else if (currentConfigIdx == 1) {
        // Keybinds config - ignore ########################, ########KEYBINDS########, ########################, and the comment line
        for (int i = 0; i < allLines.size(); ++i) {
            QString line = allLines[i].trimmed();
            if (line.startsWith("########################") || line.startsWith("########KEYBINDS########")) {
                firstConfigIdx = i + 1;
            }
        }
        // Skip the comment line after header
        if (firstConfigIdx < allLines.size() && allLines[firstConfigIdx].trimmed().startsWith("# See https://wiki.hyprland.org/Configuring/Keywords/ for more")) {
            firstConfigIdx++;
        }
    } else if (currentConfigIdx == 2) {
        // Autoruns config - ignore ########################, ########AUTORUNS########, ########################, and the comment line
        for (int i = 0; i < allLines.size(); ++i) {
            QString line = allLines[i].trimmed();
            if (line.startsWith("########################") || line.startsWith("########AUTORUNS########")) {
                firstConfigIdx = i + 1;
            }
        }
        // Skip the comment line after header
        if (firstConfigIdx < allLines.size() && allLines[firstConfigIdx].trimmed().startsWith("# Execute your favorite apps at launch")) {
            firstConfigIdx++;
        }
    } else {
        // Window Rules config - ignore ################################, ##TRANSPARENT BACKGROUND APP####, ################################, and all the example/comment lines
        for (int i = 0; i < allLines.size(); ++i) {
            QString line = allLines[i].trimmed();
            if (line.startsWith("################################") || line.startsWith("##TRANSPARENT BACKGROUND APP####")) {
                firstConfigIdx = i + 1;
            }
        }
        // Skip all the example and comment lines after header
        while (firstConfigIdx < allLines.size()) {
            QString line = allLines[firstConfigIdx].trimmed();
            if (line.startsWith("#") || line.isEmpty()) {
                firstConfigIdx++;
            } else {
                break;
            }
        }
    }
    preservedHeader = allLines.mid(0, firstConfigIdx);
    currentLines.clear();
    for (int i = firstConfigIdx; i < allLines.size(); ++i) {
        currentLines.append(allLines[i]);
    }
    rebuildLines();
}

QStringList Applications::readConfigLines(const QString &path) {
    QStringList result;
    QFile file(path);
    if (!file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text)) return result;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;
        if (line.contains("=")) result.append(line);
    }
    return result;
}

void Applications::rebuildLines() {
    // Clear old widgets
    QLayoutItem *child;
    while ((child = linesLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    lineEdits.clear();
    for (int i = 0; i < currentLines.size(); ++i) {
        QWidget *row = new QWidget(linesWidget);
        QHBoxLayout *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(20);
        QLineEdit *edit = new QLineEdit(currentLines[i], row);
        edit->setFont(QFont("monospace", 14));
        edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        edit->setStyleSheet("QLineEdit { color: white; background: transparent; border: none; margin: 0px; padding: 0px; outline: none; } QLineEdit:focus { color: white; background: transparent; border: none; margin: 0px; padding: 0px; outline: none; } QLineEdit:hover { color: white; background: transparent; border: none; margin: 0px; padding: 0px; }");
        connect(edit, &QLineEdit::editingFinished, this, [this, i, edit]() { handleLineEdited(i); });
        QPushButton *addBtn = new QPushButton("+", row);
        QPushButton *removeBtn = new QPushButton("-", row);
        addBtn->setFixedSize(50, 50);
        removeBtn->setFixedSize(50, 50);
        addBtn->setStyleSheet("QPushButton { color: #e5a7c6; background: rgba(255,255,255,0.8); border: 1px solid rgba(229,167,198,0.6); border-radius: 8px; font-size: 18px; font-weight: bold; } QPushButton:hover { background: rgba(229,167,198,0.15); color: white; }");
        removeBtn->setStyleSheet("QPushButton { color: #e57373; background: rgba(255,255,255,0.8); border: 1px solid rgba(229,103,115,0.6); border-radius: 8px; font-size: 18px; font-weight: bold; } QPushButton:hover { background: rgba(229,103,115,0.15); color: white; }");
        connect(addBtn, &QPushButton::clicked, this, [this, i]() { handleAddLine(i); });
        connect(removeBtn, &QPushButton::clicked, this, [this, i]() { handleRemoveLine(i); });
        rowLayout->addWidget(edit, 1);
        rowLayout->addWidget(addBtn);
        rowLayout->addWidget(removeBtn);
        row->setLayout(rowLayout);
        linesLayout->addWidget(row);
        lineEdits.append(edit);
    }
    linesLayout->addStretch();
}

void Applications::handleLineEdited(int idx) {
    if (idx < 0 || idx >= lineEdits.size()) return;
    currentLines[idx] = lineEdits[idx]->text();
    saveConfig();
}

void Applications::handleAddLine(int idx) {
    if (idx < 0 || idx > currentLines.size()) return;
    currentLines.insert(idx + 1, "");
    saveConfig();
    rebuildLines();
}

void Applications::handleRemoveLine(int idx) {
    if (idx < 0 || idx >= currentLines.size()) return;
    currentLines.removeAt(idx);
    saveConfig();
    rebuildLines();
}

void Applications::saveConfig() {
    QFile file(configFiles[currentConfigIdx]);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return;
    QTextStream out(&file);
    for (const QString &line : preservedHeader) out << line << "\n";
    for (const QString &line : currentLines) out << line << "\n";
    file.close();
    // Re-add watcher in case file was recreated
    if (!fileWatcher->files().contains(configFiles[currentConfigIdx]))
        fileWatcher->addPath(configFiles[currentConfigIdx]);
}

void Applications::writeConfigLines(const QString &path, const QStringList &lines) {
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) return;
    QTextStream out(&file);
    for (const QString &line : lines) out << line << "\n";
    file.close();
}

void Applications::refreshData() {
    loadConfig();
}

void Applications::handleLeftArrow() {
    currentConfigIdx = (currentConfigIdx - 1 + configFiles.size()) % configFiles.size();
    configLabel->setText(configNames[currentConfigIdx]);
    loadConfig();
}

void Applications::handleRightArrow() {
    currentConfigIdx = (currentConfigIdx + 1) % configFiles.size();
    configLabel->setText(configNames[currentConfigIdx]);
    loadConfig();
}
void Applications::show() {
    container->setVisible(true);
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (parentWidget && parentWidget->metaObject()->indexOfMethod("hideTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideTiles");
    if (parentWidget && parentWidget->metaObject()->indexOfMethod("showBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showBackButton");
}
void Applications::hide() {
    container->setVisible(false);
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (parentWidget && parentWidget->metaObject()->indexOfMethod("showTiles()") != -1)
        QMetaObject::invokeMethod(parentWidget, "showTiles");
    if (parentWidget && parentWidget->metaObject()->indexOfMethod("hideBackButton()") != -1)
        QMetaObject::invokeMethod(parentWidget, "hideBackButton");
} 