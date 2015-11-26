#ifndef GRAPHICSNODE_H
#define GRAPHICSNODE_H

#include <QGraphicsRectItem>

class GraphicsNode : public QGraphicsRectItem
{
public:

	GraphicsNode();
	~GraphicsNode();

	void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
};

#endif // !GRAPHICSNODE_H
