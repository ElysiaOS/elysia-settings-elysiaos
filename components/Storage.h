#pragma once
#include <QObject>
#include "../translations.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <QString>

class Storage : public QObject {
    Q_OBJECT
public:
    explicit Storage(QObject *parent = nullptr);
    void show();
    void hide();

private slots:
    void updateDisks();

private:
    void initUI();
    void setupTimer();
    QString bytesToGB(qint64 bytes);

    QWidget *storageBox;
    QVBoxLayout *drivesContainer;
    QList<QWidget*> driveWidgets;
    QTimer *updateTimer;
}; 