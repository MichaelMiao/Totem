#include "portitem.h"
#include "../designnetconstants.h"
#include "../designnetbase/port.h"
#include "processorgraphicsblock.h"
#include "portarrowlink.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include "data/idata.h"
#include "data/datatype.h"


namespace DesignNet
{
static PortArrowLink* _pTempLink		= 0;
static QGraphicsLineItem* _pLineItem	= 0;
//////////////////////////////////////////////////////////////////////////

PortItem::PortItem(Port* pPort, ProcessorGraphicsBlock* pBlock)
	: m_pPort(pPort), m_pBlock(pBlock)
{
	setParentItem(pBlock);
	setFlags(ItemIsFocusable | ItemIsSelectable);
	QObject::connect(pPort, SIGNAL(connectPort(Port*, Port*)), this, SLOT(portConnected(Port*, Port*)));
	QObject::connect(pPort, SIGNAL(disconnectPort(Port*, Port*)), this, SLOT(portDisconnected(Port*, Port*)));
}

PortItem::~PortItem()
{

}

QRectF PortItem::boundingRect() const
{
	QSizeF sz(PORTITEM_WIDTH, PORTITEM_WIDTH);
	return QRectF(-sz.width() / 2, -sz.height() / 2, PORTITEM_WIDTH, PORTITEM_WIDTH);
}

void PortItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/)
{
	if (isSelected())
		painter->setBrush(QBrush(QColor(255, 0, 0)));
	
	painter->drawEllipse(boundingRect());
	if (m_pPort->data())
	{
		if (m_pPort->data()->dataType < DATATYPE_USERTYPE)
		{
			painter->drawImage(boundingRect(), QImage(g_InnerDataType[m_pPort->data()->dataType].strFile));
		}
	}
}

void PortItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	setSelected(true);
	grabMouse();
	event->accept();
}

void PortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsObject::mouseMoveEvent(event);
	event->accept();
	if (m_pPort->portType() != Port::OUT_PORT)
		return;

	QPointF pt = event->scenePos();
	QPointF ptPort = this->scenePos();
	if ((pt - ptPort).manhattanLength() > 9 && !_pLineItem)
	{
		_pLineItem = new QGraphicsLineItem;
		QPen pen(Qt::green, 3, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
		_pLineItem->setPen(pen);
		scene()->addItem(_pLineItem);
	}
	if (_pLineItem)
		_pLineItem->setLine(ptPort.x(), ptPort.y(), pt.x(), pt.y());
}

void PortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	ungrabMouse();
	if (_pLineItem)
	{
		scene()->removeItem(_pLineItem);
		_pLineItem = 0;
		QPointF pt = event->scenePos();
		QGraphicsItem* pItem = scene()->itemAt(pt);
		if (pItem && pItem->type() == PortItem::Type)
			m_pPort->connect(((PortItem*)pItem)->port());
	}
}

void PortItem::portConnected(Port* src, Port* target)
{
	
}

void PortItem::portDisconnected(Port* src, Port* target)
{

}

}