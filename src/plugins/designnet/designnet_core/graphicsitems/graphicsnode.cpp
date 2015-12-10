#include "stdafx.h"
#include "graphicsnode.h"


#define NODE_SIZE	32

GraphicsNode::GraphicsNode()
{
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void GraphicsNode::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
	QRectF rc = rect();
	painter->drawEllipse(rc.left(), rc.top(), rc.width(), rc.height());
}
