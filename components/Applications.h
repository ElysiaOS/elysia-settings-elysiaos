#pragma once
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QFont>
#include <QString>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QFileSystemWatcher>
#include <QDir>

class Applications : public QObject {
    Q_OBJECT
public:
    explicit Applications(QObject *parent = nullptr);
    void show();
    void hide();

private slots:
    void refreshData();
    void handleLeftArrow();
    void handleRightArrow();
    void handleLineEdited(int idx);
    void handleAddLine(int idx);
    void handleRemoveLine(int idx);

private:
    void initUI();
    void loadConfig();
    void saveConfig();
    void rebuildLines();
    QStringList readConfigLines(const QString &path);
    void writeConfigLines(const QString &path, const QStringList &lines);

    QWidget *container;
    QVBoxLayout *mainLayout;
    QWidget *headerWidget;
    QLabel *configLabel;
    QPushButton *leftArrow;
    QPushButton *rightArrow;
    QWidget *linesWidget;
    QVBoxLayout *linesLayout;
    QFileSystemWatcher *fileWatcher;
    QFont textFont;
    int currentConfigIdx;
    QStringList configFiles;
    QStringList configNames;
    QStringList currentLines;
    QStringList allConfigLines; // Store all lines including comments
    QList<QLineEdit*> lineEdits;
    QStringList preservedHeader;
}; 