#ifndef GRAPHICSNODE_H
#define GRAPHICSNODE_H


#include <QGraphicsRectItem>
#include "designnet_core_def.h"

class GraphicsNode : public QGraphicsRectItem
{
public:

	enum { Type = DesignNet::GraphicsType_Node };

	GraphicsNode();
	~GraphicsNode() {}

	void	paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0) override;
	int		type() const override { return Type; }

	void	savePos() { m_ptOldPos = scenePos(); }
	QPointF getSavedPos() const { return m_ptOldPos; }

private:

	QPointF		m_ptOldPos;
};

#endif // !GRAPHICSNODE_H
