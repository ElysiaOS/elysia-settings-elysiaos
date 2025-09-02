#pragma once
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QVector>
#include <QString>
#include "../translations.h"

class About : public QObject {
    Q_OBJECT
public:
    explicit About(QObject *parent = nullptr);
    
    void show();
    void hide();

private:
    QStringList getFastfetchOutput();
    
    QWidget *aboutBox;
    QLabel *image;
    QVector<QLabel*> labels;
    QStringList fastfetchLines;
}; 