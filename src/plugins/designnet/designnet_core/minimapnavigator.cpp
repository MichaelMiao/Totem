#include "minimapnavigator.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QMatrix>
#include "designneteditor.h"
#include "designnetview.h"


namespace DesignNet {


MinimapNavigator::MinimapNavigator(QWidget* pParent) : QGraphicsView(pParent), m_pCurEditor(0)
{
	setInteractive(false);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setCacheMode(QGraphicsView::CacheBackground);
	m_bPressed = false;
}


MinimapNavigator::~MinimapNavigator()
{

}

void MinimapNavigator::setEditor(DesignNetEditor *pEditor)
{
	disconnect();
	m_pCurEditor = pEditor;
	if (m_pCurEditor == NULL)
		return;

	DesignNetView *editorView = static_cast<DesignNetView *>(m_pCurEditor->designNetView());
	setScene(editorView->scene());
	connect(editorView->scene(), SIGNAL(sceneRectChanged(QRectF)), this, SLOT(showScene(QRectF)));
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

void MinimapNavigator::showScene(QRectF rc)
{
	if (scene() != NULL)
	{
		DesignNetView *editorView = static_cast<DesignNetView *>(m_pCurEditor->designNetView());
		setSceneRect(editorView->sceneRect());
		fitInView(editorView->sceneRect(), Qt::KeepAspectRatio);
		scene()->update();
		update();
	}
}

void MinimapNavigator::drawForeground(QPainter *painter, const QRectF &rect)
{
	QGraphicsView::drawForeground(painter, rect);
	painter->setPen(Qt::darkYellow);
	QRect rc = m_pCurEditor->designNetView()->viewport()->rect();
	QRectF rcVisible = m_pCurEditor->designNetView()->mapToScene(rc).boundingRect();
	rcVisible.setTopLeft(rcVisible.topLeft() - QPoint(2, 2));
	rcVisible.setBottomRight(rcVisible.bottomRight() + QPoint(2, 2));
	painter->drawRect(rcVisible);
}

void MinimapNavigator::mousePressEvent(QMouseEvent * event)
{
	m_bPressed = true;
	m_pCurEditor->designNetView()->centerOn(mapToScene(event->pos()));
	scene()->update();
	QGraphicsView::mousePressEvent(event);
}

void MinimapNavigator::mouseMoveEvent(QMouseEvent *event)
{
	if (m_bPressed)
	{
		m_pCurEditor->designNetView()->centerOn(mapToScene(event->pos()));
		scene()->update();
	}
	QGraphicsView::mouseMoveEvent(event);
}

void MinimapNavigator::mouseReleaseEvent(QMouseEvent *event)
{
	m_bPressed = false;
	QGraphicsView::mouseReleaseEvent(event);
}

void MinimapNavigator::resizeEvent(QResizeEvent *event)
{
	fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
}

}
