#pragma once
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>
#include <QString>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QMap>

class Sound : public QObject {
    Q_OBJECT
public:
    explicit Sound(QObject *parent = nullptr);
    void show();
    void hide();

private slots:
    void setAudioOutput();
    void setMicInput();
    void setAudioVolume(int value);
    void setMicVolume(int value);
    void debounceAudio(int value);
    void debounceMic(int value);
    void applyChanges();
    void refreshState();
    void onAppVolumeChanged(int value);

private:
    void initUI();
    void updateSlider(QComboBox *dropdown, QSlider *slider, bool isSink);
    void updateDropdowns();
    void updatePlaybackClients();
    QStringList getDevices(const QString &type);
    QList<QPair<QString, QString>> getPortsForDevice(const QString &name, const QString &type);
    QString getActivePort(const QString &devName, const QString &type);
    QString simplifyDescription(const QString &text);
    QList<QMap<QString, QString>> getPlaybackClients();

    QWidget *soundBox;
    QLabel *audioLabel;
    QComboBox *audioDropdown;
    QSlider *audioSlider;
    QLabel *micLabel;
    QComboBox *micDropdown;
    QSlider *micSlider;
    QPushButton *applyBtn;
    QLabel *clientsLabel;
    QScrollArea *clientsScrollArea;
    QWidget *clientsWidget;
    QVBoxLayout *clientsLayout;
    QMap<QString, QSlider*> appSliders;
    QMap<QString, QTimer*> appTimers;
    QTimer *refreshTimer;
    QTimer *audioTimer;
    QTimer *micTimer;
}; 