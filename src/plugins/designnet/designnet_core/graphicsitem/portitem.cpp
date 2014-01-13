#include "portitem.h"
#include "../designnetconstants.h"
#include "../designnetbase/port.h"
#include "processorgraphicsblock.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
namespace DesignNet
{

//////////////////////////////////////////////////////////////////////////

PortItem::PortItem(Port* pPort, ProcessorGraphicsBlock* pBlock)
	: m_pPort(pPort), m_pBlock(pBlock)
{
	setParentItem(pBlock);
	setFlags(ItemIsFocusable | ItemIsSelectable);
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
	{
		painter->setBrush(QBrush(QColor(255, 0, 0)));
	}
	painter->drawEllipse(boundingRect());
}

void PortItem::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	setSelected(true);
	grabMouse();
	event->accept();
}

void PortItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF pt = event->pos();

}

void PortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	ungrabMouse();
}

}