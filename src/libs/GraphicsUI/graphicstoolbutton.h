#ifndef GRAPHICSTOOLBUTTON_H
#define GRAPHICSTOOLBUTTON_H

#include <QGraphicsObject>
#include "graphicsui_global.h"
namespace GraphicsUI{
class GraphicsToolButtonPrivate;
class TOTEM_GRAPHICSUI_EXPORT GraphicsToolButton : public QGraphicsObject
{
	Q_OBJECT
public:
    GraphicsToolButton(QGraphicsItem *parent = 0);
	virtual ~GraphicsToolButton();

	virtual QRectF boundingRect () const;
	virtual void paint(QPainter * painter,
		const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) ;

	void mousePressEvent( QGraphicsSceneMouseEvent * event );
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	void setSize(QSize size);
	QSize size();
signals:
	void clicked();
private:
	GraphicsToolButtonPrivate *d;
};
}

#endif // GRAPHICSTOOLBUTTON_H
