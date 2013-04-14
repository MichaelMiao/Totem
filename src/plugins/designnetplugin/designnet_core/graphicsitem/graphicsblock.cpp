#include "graphicsblock.h"
#include "../designnetbase/block.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"
#include "designnetconstants.h"

#include <QApplication>
#include <QPainter>
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QFont>
#include <QTimer>
using namespace GraphicsUI;
namespace DesignNet{
GraphicsBlock::GraphicsBlock(int iWidth, int iHeight,
                             QGraphicsItem *parent)
    : QGraphicsObject(parent),m_size(iWidth, iHeight)
{
    setFlag(ItemIsMovable);
    setFlag(ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
    ///
    /// close按钮
    m_closeItem = new GraphicsAutoShowHideItem(this);
    m_closeItem->setSize(QSize(16, 16));
    m_closeItem->setPos(boundingRect().right() - 16, boundingRect().top());
    m_closeItem->setPixmap(QPixmap(":/media/item-close.png"));
    connect(m_closeItem, SIGNAL(clicked()), this, SIGNAL(closed()));
    ///
    /// title
    m_titleItem = new QGraphicsTextItem(this);
    QTimer::singleShot(0, this, SLOT(relayout()));
}

GraphicsBlock::~GraphicsBlock()
{
}

void GraphicsBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    layoutItems();
    setTitle(title());
    m_titleItem->setPos(boundingRect().left(), boundingRect().top());
    QRectF rectF = boundingRect();
    rectF.adjust(2, 2, -2, -2);

    painter->setPen(Qt::NoPen);
    QRectF rectShadow = rectF;
    rectShadow.translate(5, 5);
    painter->fillRect(rectShadow, qApp->palette().shadow().color());

    painter->fillRect(rectF, QColor(102, 175,253));
    if(isSelected())
    {
        painter->save();
        painter->setPen(QColor(255, 255, 255));
        QRectF rect = boundingRect();
        rect.adjust(2, 2, -2, -2);
        painter->drawRect(rect);

        painter->setPen(QPen(Qt::darkBlue, 2));
        rect = boundingRect();
        painter->drawRect(rect);
        painter->restore();
        this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Emphasize);
    }
    else
    {
        this->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Normal);
    }
    painter->save();
    painter->setPen(QPen(Qt::white, 1));
    painter->drawLine(rectF.left(), rectF.top() + TITLE_HEIGHT,
                      rectF.right(), rectF.top() + TITLE_HEIGHT);
    painter->setRenderHints(QPainter::Antialiasing);
    painter->restore();
}

void GraphicsBlock::setTitle(const QString &title)
{
    QString strTitle = "<span style=\"font-size:10pt;color:white\"> %1 </span>";
    m_titleItem->setHtml(strTitle.arg(title));
}

QRectF GraphicsBlock::boundingRect() const
{
    return QRectF(- m_size.width() * 1.0/ 2,
                  - m_size.height() * 1.0 /2,
                  m_size.width(),
                  m_size.height());
}

QSizeF GraphicsBlock::minimumSizeHint() const
{
    float fHeight = 1.0*TITLE_HEIGHT;
    return QSizeF(m_size.width(), qMax((float)m_size.height(), fHeight));
}

QRectF GraphicsBlock::mainRect() const
{
    QRectF rectF = boundingRect();
    return QRectF(rectF.left(), rectF.top() + TITLE_HEIGHT + 1,
                  rectF.width(), rectF.height() - TITLE_HEIGHT - 1);
}

void GraphicsBlock::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    m_closeItem->setPos(boundingRect().right() - 16, boundingRect().top());
    m_closeItem->animateShow(true);
}

void GraphicsBlock::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    m_closeItem->animateShow(false);
}

void GraphicsBlock::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void GraphicsBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}

void GraphicsBlock::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    this->update();
    QGraphicsItem::mouseMoveEvent(event);
}

void GraphicsBlock::relayout()
{
    layoutItems();
    update();
}

}
