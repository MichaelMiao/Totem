#include "blocktextitem.h"
#include <QPainter>
namespace DesignNet{
BlockTextItem::BlockTextItem(QGraphicsItem *parent)
	: QGraphicsTextItem(parent)
{

}

BlockTextItem::~BlockTextItem()
{

}

void BlockTextItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
	QRectF rectF = boundingRect();
	painter->save();
	painter->setBrush(Qt::black);
	painter->drawRoundedRect(rectF, 10, 15);
	painter->restore();
	QGraphicsTextItem::paint(painter, option, widget);
	
}

}
