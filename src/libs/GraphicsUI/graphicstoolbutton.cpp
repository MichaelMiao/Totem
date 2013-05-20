#include "graphicstoolbutton.h"
namespace GraphicsUI{

class GraphicsToolButtonPrivate{
public:
	GraphicsToolButtonPrivate(GraphicsToolButton *button)
		: q(button)
	{
		
	}
	GraphicsToolButton *q;
};
GraphicsToolButton::GraphicsToolButton(QGraphicsItem *parent)
	: QGraphicsObject(parent),
	d(new GraphicsToolButtonPrivate(this))
{
}

GraphicsToolButton::~GraphicsToolButton()
{
	if (d)
	{
		delete d;
		d = 0;
	}
}

void GraphicsToolButton::mousePressEvent( QGraphicsSceneMouseEvent * event )
{

}

void GraphicsToolButton::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{

}

void GraphicsToolButton::setSize( QSize size )
{

}

QSize GraphicsToolButton::size()
{
	return QSize(0, 0);
}

QRectF GraphicsToolButton::boundingRect() const
{
	return QRectF(0, 0, 0, 0);
}

void GraphicsToolButton::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0 */ )
{

}

}
