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

static QList<QGraphicsItem *> GetItemByType(const QList<QGraphicsItem *>& listItem, DesignNet::GraphicsItemType eType)
{
	QList<QGraphicsItem *> vecRes;
	for (QGraphicsItem * pItem : listItem)
	{
		if (pItem->type() == (int)eType)
			vecRes.push_back(pItem);
	}
	return std::move(vecRes);
}

void DesignNetView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_ptMouseDown = mapToScene(event->pos());
		QRectF rc(m_ptMouseDown, QSize(0, 0));
		rc.adjusted(-CLICK_VALID_SIZE, -CLICK_VALID_SIZE, CLICK_VALID_SIZE, CLICK_VALID_SIZE);
		QList<QGraphicsItem *> itemList = scene()->items(rc);
		QList<QGraphicsItem *> vecItems = GetItemByType(itemList, DesignNet::GraphicsType_Node);
		if (!vecItems.empty())
		{
			QGraphicsItem* pItem = vecItems.at(0);
		}
	}
	return QGraphicsView::mousePressEvent(event);
}

void DesignNetView::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::MidButton)
	{
		GraphicsNode* pNode = new GraphicsNode;
		pNode->setPos(0, 0);
		pNode->setRect(0, 0, 100, 100);
		scene()->addItem(pNode);
	}
	else if (event->button() == Qt::LeftButton)
	{
		m_ptMouseDown = QPoint(0, 0);
	}
	return QGraphicsView::mouseReleaseEvent(event);
}

void DesignNetView::mouseMoveEvent(QMouseEvent *event)
{
	QPointF ptMouse = mapToScene(event->pos());
	QList<GraphicsNode*> vecSelected = GetSelectedNode();
	for (GraphicsNode* pItem : vecSelected)
	{
//		pItem->setPos(pItem->scenePos());
	}
	return QGraphicsView::mouseMoveEvent(event);
}

QList<GraphicsNode*> DesignNetView::GetSelectedNode()
{
	QList<GraphicsNode*> vecNode;
	QList<QGraphicsItem *> vecSelected = scene()->selectedItems();
	for (QGraphicsItem* pItem : vecSelected)
	{
		if (pItem->type() == DesignNet::GraphicsType_Node)
			vecNode.push_back((GraphicsNode*)pItem);
	}
	return std::move(vecNode);
}
