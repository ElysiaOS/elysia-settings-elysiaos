#include "AnimatedTile.h"
#include <QDir>
#include <QFont>
#include <QIcon>
#include <QTransform>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QRandomGenerator>
#include <QFile>
#include <QDebug>

AnimatedTile::AnimatedTile(QWidget *parent, const QString &imgPath, const QString &labelText, 
                           const QPoint &tilePos, const QPoint &labelPos, 
                           const QString &effectType, std::function<void()> clickHandler, bool isCar)
    : QPushButton(parent), effectType(effectType), basePos(tilePos), clickHandler(clickHandler), 
      isCar(isCar), bounceDir(1), bounceOffset(0), isDestroyed(false), opacity(1.0) {
    
    setMouseTracking(true);
    
    QString fullImgPath = QDir::homePath() + "/.config/Elysia/assets/assets/" + imgPath;
    if (!QFile::exists(fullImgPath)) {
        qWarning() << "[AnimatedTile] Image file does not exist:" << fullImgPath;
    }
    originalPixmap = QPixmap(fullImgPath).scaled(130, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setIcon(QIcon(originalPixmap));
    setIconSize(originalPixmap.size());
    resize(originalPixmap.size());
    setStyleSheet("border: none; background: transparent;");
    move(basePos);
    
    glass = new QLabel(parent);
    glass->setGeometry(geometry());
    glass->setStyleSheet("background-color: rgba(255, 255, 255, 0.08); border-radius: 20px;");
    glass->lower();
    
    label = new QLabel(labelText, parent);
    QFont font("ElysiaOSNew12", 14, QFont::Bold);
    label->setFont(font);
    label->setStyleSheet("color: #db11fa; background: transparent;");
    label->adjustSize();
    label->move(labelPos);
    
    animTimer = new QTimer(this);
    animTimer->setInterval(16);
    connect(animTimer, &QTimer::timeout, this, &AnimatedTile::updateEffect);
    
    connect(this, &QPushButton::clicked, this, &AnimatedTile::handleClick);
}

void AnimatedTile::enterEvent(QEnterEvent *event) {
    if (!isDestroyed) {
        animTimer->start();
        bounceDir = 1;
    }
    QPushButton::enterEvent(event);
}

void AnimatedTile::leaveEvent(QEvent *event) {
    if (!isDestroyed) {
        animTimer->stop();
        setIcon(QIcon(originalPixmap));
        setIconSize(originalPixmap.size());
        resize(originalPixmap.size());
        move(basePos);
        glass->setGeometry(geometry());
    }
    QPushButton::leaveEvent(event);
}

void AnimatedTile::mouseMoveEvent(QMouseEvent *event) {
    if (!isDestroyed) {
        mouseOffset = event->position().toPoint();
    }
    QPushButton::mouseMoveEvent(event);
}

void AnimatedTile::updateEffect() {
    if (isDestroyed) return;
    
    if (effectType == "rotate") {
        double angle = (mouseOffset.x() - width() / 2.0) * 0.1;
        QTransform transform;
        transform.rotate(angle);
        QPixmap rotated = originalPixmap.transformed(transform, Qt::SmoothTransformation);
        setIcon(QIcon(rotated));
        setIconSize(rotated.size());
    }
    
    bounceOffset += bounceDir * 0.7;
    if (bounceOffset > 8 || bounceOffset < -8) {
        bounceDir *= -1;
    }
    
    int newY = basePos.y() + bounceOffset;
    move(basePos.x(), newY);
    glass->move(x(), y());
}

void AnimatedTile::handleClick() {
    if (clickHandler) {
        clickHandler();
    }
}

void AnimatedTile::destroyWithFall() {
    if (isDestroyed) return;
    
    isDestroyed = true;
    animTimer->stop();
    
    fallAnimation = new QPropertyAnimation(this, "pos");
    fallAnimation->setDuration(1200);
    fallAnimation->setStartValue(pos());
    
    int endX = x() + QRandomGenerator::global()->bounded(-100, 101);
    int endY = parentWidget()->height() + 100;
    fallAnimation->setEndValue(QPoint(endX, endY));
    fallAnimation->setEasingCurve(QEasingCurve::InQuad);
    fallAnimation->start();
    
    fadeTimer = new QTimer(this);
    fadeTimer->setInterval(30);
    opacity = 1.0;
    connect(fadeTimer, &QTimer::timeout, this, &AnimatedTile::fadeOut);
    fadeTimer->start();
    
    glass->hide();
    label->hide();
}

void AnimatedTile::fadeOut() {
    opacity -= 0.03;
    if (opacity <= 0) {
        fadeTimer->stop();
        hide();
        return;
    }
    setStyleSheet(QString("border: none; background: transparent; opacity: %1;").arg(opacity));
} 