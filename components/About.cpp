#include "About.h"
#include <QDir>
#include <QFont>
#include <QPixmap>
#include <QProcess>
#include <QWidget>
#include <QLabel>

About::About(QObject *parent) : QObject(parent) {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent);
    if (!parentWidget) return;
    
    aboutBox = new QWidget(parentWidget);
    aboutBox->setGeometry(500, 100, 600, 640);
    aboutBox->setStyleSheet(
        "background-color: rgba(255, 255, 255, 0.05);"
        "border: 2px solid rgba(255, 255, 255, 0.15);"
        "border-radius: 30px;"
    );
    aboutBox->setVisible(false);
    
    // Desktop image
    image = new QLabel(aboutBox);
    QPixmap desktopPixmap(QDir::homePath() + "/.config/Elysia/assets/assets/desktop.png");
    image->setPixmap(desktopPixmap.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    image->setGeometry(0, 0, 580, 170);
    image->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    image->setAlignment(Qt::AlignCenter);
    
    // Title
    QLabel *title = new QLabel("SYSTEM INFORMATION", aboutBox);
    QFont titleFont("ElysiaOSNew", 20, QFont::Bold);
    title->setFont(titleFont);
    title->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
    title->setAlignment(Qt::AlignCenter);
    title->setGeometry(155, 90, 290, 170);
    
    // Fastfetch output labels
    fastfetchLines = getFastfetchOutput();
    int y = 210;
    for (const QString &line : fastfetchLines) {
        QLabel *label = new QLabel(line.trimmed(), aboutBox);
        QFont font("ElysiaOSNew22", 16);
        label->setFont(font);
        label->setStyleSheet("color: white; background: transparent; border: none; margin: 0px; padding: 0px");
        label->setGeometry(70, y, 480, 30);
        label->setWordWrap(true);
        label->setAlignment(Qt::AlignLeft);
        label->setVisible(true);
        labels.append(label);
        y += 30;
    }
}

QStringList About::getFastfetchOutput() {
    QStringList result;
    QProcess process;
    QStringList args;
    args << "-c" << QDir::homePath() + "/.config/fastfetch/sys-config.jsonc";
    
    process.start("fastfetch", args);
    if (process.waitForFinished()) {
        QString output = QString::fromUtf8(process.readAllStandardOutput());
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            if (!line.trimmed().isEmpty()) {
                result.append(line);
            }
        }
    } else {
        result.append("Error: Failed to run fastfetch");
    }
    
    return result;
}

void About::show() {
    QWidget *parentWidget = qobject_cast<QWidget*>(parent());
    if (parentWidget) {
        // Hide tiles and show back button (would need to be implemented in MainWindow)
        aboutBox->setVisible(true);
    }
}

void About::hide() {
    aboutBox->setVisible(false);
    // Show tiles and hide back button (would need to be implemented in MainWindow)
} 