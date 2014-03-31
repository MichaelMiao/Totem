#include "portitem.h"
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include "../data/datatype.h"
#include "../data/idata.h"
#include "../designnetbase/port.h"
#include "../widgets/tooltipgraphicsitem.h"
#include "../designnetconstants.h"
#include "portarrowlink.h"
#include "processorgraphicsblock.h"


using namespace GraphicsUI;
namespace DesignNet
{
static PortArrowLink* _pTempLink		= 0;
static QGraphicsLineItem* _pLineItem	= 0;
//////////////////////////////////////////////////////////////////////////

PortItem::PortItem(Port* pPort, ProcessorGraphicsBlock* pBlock)
	: GraphicsToolButton(pBlock), m_pPort(pPort), m_pBlock(pBlock)
{
	setParentItem(pBlock);
	setFlags(ItemIsFocusable | ItemIsSelectable);
	
	m_pToolTip = new ToolTipGraphicsItem(this);
	m_pToolTip->hide();
	QObject::connect(pPort, SIGNAL(connectPort(Port*, Port*)), this, SLOT(onPortConnected(Port*, Port*)));
	QObject::connect(pPort, SIGNAL(disconnectPort(Port*, Port*)), this, SLOT(onPortDisconnected(Port*, Port*)));
	QObject::connect(pPort, SIGNAL(dataChanged()), this, SLOT(onPortDataChanged()));
}

PortItem::~PortItem()
{

}

int PortItem::processorId()
{
	return m_pPort->processor()->id();
}

int PortItem::index()
{
	return m_pPort->getIndex();
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
			painter->drawImage(boundingRect(), QImage(g_InnerDataType[m_pPort->data()->dataType].strFile));
	}
}

void PortItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	setSelected(true);
	event->accept();
	GraphicsToolButton::mousePressEvent(event);
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
	GraphicsToolButton::mouseReleaseEvent(event);
	if (_pLineItem)
	{
		scene()->removeItem(_pLineItem);
		_pLineItem = 0;
		QPointF pt = event->scenePos();
		QGraphicsItem* pItem = scene()->itemAt(pt);
		if (pItem && pItem->type() == PortItem::Type)
			m_pPort->connect(((PortItem*)pItem)->port());
	}
	else if (event->button() == Qt::LeftButton && m_pPort->portType() == Port::OUT_PORT)
	{
		m_pToolTip->setData(m_pPort->data()->variant);
		m_pToolTip->show();
	}
}

void PortItem::onPortConnected(Port* src, Port* target)
{
	
}

void PortItem::onPortDisconnected(Port* src, Port* target)
{

}

void PortItem::onPortDataChanged()
{
	m_pToolTip->setData(m_pPort->data()->variant);
}

void PortItem::hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
	QString toolTip;
	toolTip = tr("<p><b>PortName:</b> <span style=\"color: red; font-size: small\">%2</span></p>").arg(m_pPort->name());
	if (m_pPort->portType() == Port::IN_PORT)
	{
		toolTip += tr("<b>PortType:</b> <span style=\"color: red; font-size: small\">IN_PORT</span>");	
	}
	else
		toolTip += tr("<b>PortType:</b> <span style=\"color: red; font-size: small\">OUT_PORT</span>");

	setToolTip(toolTip);

	update();
	QGraphicsItem::hoverEnterEvent(event);
}

void PortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
	update();
	QGraphicsItem::hoverLeaveEvent(event);
}

QImage PortItem::getImage()
{
	return QImage(g_InnerDataType[m_pPort->data()->dataType].strFile);
}

}
