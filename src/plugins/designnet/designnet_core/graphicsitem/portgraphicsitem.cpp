#include "portgraphicsitem.h"
#include "../designnetconstants.h"
#include "../designnetbase/port.h"
#include "portarrowlinkitem.h"
#include "processorgraphicsblock.h"
#include "tooltipgraphicsitem.h"
#include "designnetspace.h"
#include "GraphicsUI/graphicsautoshowhideitem.h"
#include "coreplugin/icore.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QEvent>
#include <QDrag>
using namespace GraphicsUI;
namespace DesignNet{

PortGraphicsItem::PortGraphicsItem(Port *port, ProcessorGraphicsBlock* processorBlock) :
    QGraphicsObject(processorBlock),
    m_port(port),
	m_size(DEFAULT_PORT_WIDTH, DEFAULT_PORT_HEIGHT),
	m_processorBlock(processorBlock)
{
    m_bPressed = false;
	m_bMouseOver = false;
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
	setAcceptDrops(true);
	setCacheMode(QGraphicsItem::ItemCoordinateCache);
    m_imageConnected.load(Constants::ITEM_IMAGE_PORT_CONNECTED);
    m_imageNotConnected.load(Constants::ITEM_IMAGE_PORT_NOTCONNECTED);
    m_toolTipItem = new ToolTipGraphicsItem(this);
    m_toolTipItem->hide();
	m_typeImageItem = new GraphicsAutoShowHideItem(this);
	m_typeImageItem->setPixmap(QPixmap::fromImage(port->data()->image()));
	m_typeImageItem->setSize(QSize(16, 16));
	m_typeImageItem->animateShow(false);
	QObject::connect(m_port, SIGNAL(disconnectFromPort(Port*)), this, SLOT(onDisconnectFromPort(Port*)));
}

PortGraphicsItem::~PortGraphicsItem()
{

}

QRectF PortGraphicsItem::boundingRect() const
{
	float fWidth = DEFAULT_PORT_WIDTH ;
	float fHeight = DEFAULT_PORT_HEIGHT;
	return QRectF(-fWidth/2, -fHeight/2, fWidth, fHeight);
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
        painter->drawPixmap(QRectF(rect.left(), rect.top(), DEFAULT_PORT_WIDTH, DEFAULT_PORT_HEIGHT), m_imageConnected, m_imageConnected.rect());
    }
    else
    {
        painter->drawPixmap(QRectF(rect.left(), rect.top(), DEFAULT_PORT_WIDTH, DEFAULT_PORT_HEIGHT), m_imageNotConnected, m_imageNotConnected.rect());
    }	
}

ProcessorGraphicsBlock *PortGraphicsItem::processor()
{
    return m_processorBlock;
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
	QString toolTip;
	QString strPattern = tr("<b>%1</b> <span style=\"color: red; font-size: small\">%2</span>");
	toolTip = tr("<p><b>PortName:</b> <span style=\"color: red; font-size: small\">%2</span></p>").arg(m_port->name());
	QString sTitle, sMark;
	sTitle = tr("PortType: ");
	sMark = m_port->portType() == Port::IN_PORT ? tr("IN_PORT") : tr("OUT_PORT");

	toolTip += strPattern.arg(sTitle).arg(sMark);

	sMark	= m_port->marks();
	if (!sMark.isEmpty())
	{
		sTitle	= tr("Data Marks");
		toolTip += strPattern.arg(sTitle).arg(sMark);
	}

	setToolTip(toolTip);
	
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
	if(event->button() == Qt::LeftButton)
	{
		m_bPressed = true;
		this->translate(1, 1);
		
		event->accept();
		grabMouse();
	}
//	QGraphicsItem::mousePressEvent(event);
}

void PortGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	
    if(m_bPressed && getPort()->portType() == Port::OUT_PORT &&
            QLineF(event->screenPos(), event->buttonDownScreenPos(Qt::LeftButton))
            .length() < QApplication::startDragDistance())
    {
		event->accept();
    }
	setOpacity(0.5);
	QGraphicsItem::mouseMoveEvent(event);
}

void PortGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_bPressed)
    {
        m_bPressed = false;
        this->translate(-1, -1);
		setOpacity(1);
		ungrabMouse();
    }
    if(event->button() == Qt::RightButton)
    {
        m_toolTipItem->setVisible(!m_toolTipItem->isVisible());
        event->accept();
        return ;
    }
	event->accept();
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

void PortGraphicsItem::dragEnterEvent( QGraphicsSceneDragDropEvent * event )
{
	event->setAccepted(true);
	QMessageBox box;
	box.exec();
}

void PortGraphicsItem::showTypeImage( TypeImageDirection direction )
{
	if(direction == LEFT)
	{
		m_typeImageItem->setPos(boundingRect().left() - m_typeImageItem->boundingRect().width() - DEFAULT_SPACING,
			boundingRect().top());
		m_typeImageItem->animateShow(true);
	}
	else if (direction == RIGHT)
	{
		m_typeImageItem->setPos(boundingRect().right() + DEFAULT_SPACING,
			boundingRect().top());
		m_typeImageItem->animateShow(true);
	}
}

void PortGraphicsItem::setTypeImageVisible( bool bVisible /*= true*/ )
{
	m_typeImageItem->animateShow(bVisible);
}

void PortGraphicsItem::setMouseOver( bool bOver )
{
	m_bMouseOver = bOver; 
	update();
}

void PortGraphicsItem::onDisconnectFromPort( Port *port )
{
	update();
}

void PortGraphicsItem::showTooltip( bool bShow /*= true*/ )
{

}


}
