#include "stdafx.h"
#include "designnetview.h"
#include <QMouseEvent>
#include <QGraphicsScene>
#include "graphicsitems/graphicsnode.h"


#define CLICK_VALID_SIZE 2

DesignNetView::DesignNetView(QWidget *parent) : QGraphicsView(parent)
{
	setRenderHint(QPainter::Antialiasing);
	ensureVisible(0, 0, 1, 1);				// ¾ÓÖÐÏÔÊ¾
	viewport()->setMouseTracking(true);
	QGraphicsScene* pScene = new QGraphicsScene(this);
	setScene(pScene);
}

void DesignNetView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		QPointF ptMouseDown = mapToScene(event->pos());
		QRectF rc(ptMouseDown, QSize(0, 0));
		rc.adjusted(-CLICK_VALID_SIZE, -CLICK_VALID_SIZE, CLICK_VALID_SIZE, CLICK_VALID_SIZE);
		QList<QGraphicsItem *> itemList = scene()->items(rc);
		if (!itemList.empty())
		{
			QGraphicsItem * pItem = itemList.at(0);

		}
	}
}

void DesignNetView::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->buttons().testFlag(Qt::MidButton))
	{
		GraphicsNode* pNode = new GraphicsNode;
		pNode->setPos(0, 0);
		pNode->setRect(0, 0, 100, 100);
		scene()->addItem(pNode);
	}
}
