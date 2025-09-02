#pragma once
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QPoint>
#include <QString>
#include <functional>

class AnimatedTile : public QPushButton {
    Q_OBJECT
public:
    AnimatedTile(QWidget *parent, const QString &imgPath, const QString &labelText, 
                 const QPoint &tilePos, const QPoint &labelPos, 
                 const QString &effectType = "rotate", 
                 std::function<void()> clickHandler = nullptr, 
                 bool isCar = false);
    
    void destroyWithFall();
    QString theme; // Theme property ("elysian" or "amphoreus")
    QLabel* getLabel() { return label; }
    QLabel* getGlass() { return glass; }
    bool isDestroyed;

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private slots:
    void updateEffect();
    void fadeOut();

private:
    void handleClick();

    QLabel *glass;
    QLabel *label;
    QTimer *animTimer;
    QPropertyAnimation *fallAnimation;
    QTimer *fadeTimer;
    
    QPixmap originalPixmap;
    QPoint basePos;
    QPoint mouseOffset;
    QString effectType;
    QString originalImageName;
    
    int bounceDir;
    double bounceOffset;
    double opacity;
    bool isCar;
    
    std::function<void()> clickHandler;
}; 