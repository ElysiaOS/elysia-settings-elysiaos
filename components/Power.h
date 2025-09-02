#pragma once
#include <QObject>
#include "../translations.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QStackedLayout>
#include <QString>
#include <QMap>
#include <QFutureWatcher>

class Power : public QObject {
    Q_OBJECT
public:
    explicit Power(QObject *parent = nullptr);
    void show();
    void hide();

private slots:
    void selectMode(const QString &mode);
    void applyChanges();

private:
    void initUI();
    void autoDetectCurrentMode();
    QFutureWatcher<QString> *modeDetectWatcher = nullptr;
    void onModeDetectFinished();
    protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    QWidget *powerBox;
    QPushButton *applyBtn;
    QString selectedMode;
    QMap<QString, QPair<QFrame*, QLabel*>> modeBoxes;
}; 