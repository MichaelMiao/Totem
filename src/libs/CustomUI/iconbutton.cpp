#include "iconbutton.h"

#include <QPainter>
#include <QStyle>
#include <QPaintEvent>
#include <QPropertyAnimation>

using namespace CustomUI;

#define ICONBUTTON_HEIGHT 18
#define FADE_TIME 160

IconButton::IconButton(QWidget *parent) :
    QAbstractButton(parent), m_autoHide(false)
{
    setCursor(Qt::ArrowCursor);
    setFocusPolicy(Qt::NoFocus);
}

void IconButton::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect pixmapRect = QRect(0, 0, m_pixmap.width(), m_pixmap.height());

    if (m_autoHide)
        painter.setOpacity(m_iconOpacity);

    painter.drawPixmap(rect(), m_pixmap, pixmapRect);
}

void IconButton::animateShow(bool visible)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "iconOpacity");
    animation->setDuration(FADE_TIME);
    animation->setEndValue(visible ? 1.0 : 0.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

}
