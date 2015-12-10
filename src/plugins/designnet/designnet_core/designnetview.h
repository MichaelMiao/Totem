#pragma once

#include <QGraphicsView>

class GraphicsNode;
class DesignNetView : public QGraphicsView
{
	Q_OBJECT

public:

	explicit DesignNetView(QWidget *parent = 0);

private:

	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent * event);
	void mouseMoveEvent(QMouseEvent *event);

	QList<GraphicsNode*> GetSelectedNode();

	QPointF m_ptMouseDown;
};
