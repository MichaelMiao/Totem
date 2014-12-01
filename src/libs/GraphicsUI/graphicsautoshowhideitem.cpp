#include "graphicsautoshowhideitem.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

namespace GraphicsUI{

GraphicsAutoShowHideItem::GraphicsAutoShowHideItem(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    m_iFadeSpeed = 60;
    m_autoHide  = true;

    setAcceptHoverEvents(true);
    setCacheMode(QGraphicsItem::ItemCoordinateCache);
    m_animGroup = new QParallelAnimationGroup(this);
    m_animFade  = new QPropertyAnimation(this, "iconOpacity");
    m_animGroup->addAnimation(m_animFade);
    m_animFade->setDuration(m_iFadeSpeed);
	m_bEableButtonStyle = true;
	m_size = QSize(16, 16);
}


void GraphicsAutoShowHideItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->moveBy(1, 1);
}

void GraphicsAutoShowHideItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	this->moveBy(-1, -1);
    emit clicked();
	event->accept();
}

void GraphicsAutoShowHideItem::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;
    update();
}

QPixmap GraphicsAutoShowHideItem::pixmap()
{
    return m_pixmap;
}

void GraphicsAutoShowHideItem::setSpeed(int iSpeed)
{
    m_iFadeSpeed = iSpeed;
}


void GraphicsAutoShowHideItem::animateShow(bool visible)
{
    if(visible)
    {
        m_animFade->setStartValue(opacity());
        m_animFade->setEndValue(1);
        m_animGroup->start();
    }
    else
    {
        m_animFade->setStartValue(opacity());
        m_animFade->setEndValue(0);
        m_animGroup->start();
    }
}

QRectF GraphicsAutoShowHideItem::boundingRect() const
{
    return QRectF(0, 0, m_size.width(), m_size.height());
}

void GraphicsAutoShowHideItem::paint(QPainter *painter,
                                     const QStyleOptionGraphicsItem *option,
                                     QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QRect pixmapRect = QRect(0, 0, m_size.width(), m_size.height());
    if(!m_pixmap.isNull())
    {

        if (m_autoHide)
            painter->setOpacity(m_iconOpacity);

        painter->drawPixmap(pixmapRect, m_pixmap);
    }
}
}
