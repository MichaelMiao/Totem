#include "portgraphicsitem.h"
#include "../designnetconstants.h"
#include "../designnetbase/port.h"
#include "portarrowlinkitem.h"
#include "processorgraphicsblock.h"
#include "tooltipgraphicsitem.h"
#include "designnetspace.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QEvent>
namespace DesignNet{

PortGraphicsItem::PortGraphicsItem(Port *port, QGraphicsItem *parent) :
    QGraphicsObject(parent),
    m_size(DEFAULT_WIDTH, DEFAULT_HEIGHT),
    m_port(port),
    m_movingArrow(0)
{
    m_bPressed = false;
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    m_imageConnected.load(Constants::ITEM_IMAGE_PORT_CONNECTED);
    m_imageNotConnected.load(Constants::ITEM_IMAGE_PORT_NOTCONNECTED);
    m_toolTipItem = new ToolTipGraphicsItem(this);
    m_toolTipItem->setText("miao");
    m_toolTipItem->hide();
}

PortGraphicsItem::~PortGraphicsItem()
{

}

QRectF PortGraphicsItem::boundingRect() const
{
    return QRectF(-m_size.width() *1.0/2, -m_size.height()*1.0/2,
                  m_size.width(), m_size.width());
}

void PortGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                             QWidget *widget)
{
    Q_UNUSED(widget);
    QRectF rect = boundingRect();
    QRectF rectRegion = boundingRect().adjusted(1, 1, -1, -1);
    if(option->state & QStyle::State_MouseOver)
    {
        painter->save();
        painter->setPen(Qt::white);
        painter->setBrush(QBrush(QColor(255, 0, 255, 200)));
        painter->drawRoundRect(rectRegion);
        painter->restore();
    }

    if(m_port->isConnected())
    {
        painter->drawPixmap(rect, m_imageConnected, m_imageConnected.rect());
    }
    else
    {
        painter->drawPixmap(rect, m_imageNotConnected, m_imageNotConnected.rect());
    }
}

void PortGraphicsItem::setMovingArrow(PortArrowLinkItem *item)
{
    m_movingArrow = item;
}

ProcessorGraphicsBlock *PortGraphicsItem::processor()
{
    return (ProcessorGraphicsBlock*)(getPort()->processor());
}

void PortGraphicsItem::removeAllConnection()
{
    DesignNetSpace *space = designNetSpace();
    foreach(Port* port, m_port->connectedPorts())
    {
        if(port->portType() == Port::OUT_PORT)
        {
            space->disconnectPort(m_port, port);
        }
        else
        {
            space->disconnectPort(port, m_port);
        }

    }
}

DesignNetSpace *PortGraphicsItem::designNetSpace() const
{
    return getPort()->processor()->space();
}

void PortGraphicsItem::updateData()
{
    
}

void PortGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    update();
    QGraphicsItem::hoverEnterEvent(event);
}

void PortGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

void PortGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_bPressed = true;
    this->translate(1, 1);
    event->accept();
}

void PortGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_bPressed &&
            QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
            .length() < QApplication::startDragDistance())
    {
        return;
    }
    else if(m_bPressed)
    {
        if(m_port->portType() == Port::OUT_PORT && m_movingArrow == 0)
        {
            m_movingArrow = new PortArrowLinkItem(this);
            m_movingArrow->setSourcePort(this);
            scene()->addItem(m_movingArrow);
            return ;
        }
    }
    if(m_movingArrow)
    {
        m_movingArrow->setEndPoint(event->scenePos());
        event->accept();
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void PortGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_bPressed)
    {
        m_bPressed = false;
        this->translate(-1, -1);
    }
    if(m_movingArrow)
    {
        delete m_movingArrow;
        m_movingArrow = 0;
    }
    processor()->setSelected(true);
    if(event->button() == Qt::RightButton)
    {
        m_toolTipItem->setVisible(!m_toolTipItem->isVisible());
        event->ignore();
        return ;
    }
    else if(event->button() == Qt::LeftButton)
    {
        QGraphicsItem *item = scene()->itemAt(event->scenePos());
        if(item && item->type() == PortGraphicsItem::Type)
        {
            PortGraphicsItem* targetPort = static_cast<PortGraphicsItem*>(item);
            if(targetPort)
            {
                processor()->connect(targetPort, this);
            }
        }
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

int PortGraphicsItem::type() const
{
    return Type;
}

void PortGraphicsItem::processorSelectionChanged(bool value)
{
    if(!value && !m_toolTipItem->topmost())
    {
        m_toolTipItem->setVisible(false);
    }
}

}
