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
	setRenderHint(QPainter::Antialiasing, true);

	setInteractive(false);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	m_bPressed = false;
}


MinimapNavigator::~MinimapNavigator()
{

}

void MinimapNavigator::setEditor(DesignNetEditor *pEditor)
{
	m_pCurEditor = pEditor;

	if (m_pCurEditor == NULL)
		return;

	DesignNetView *editorView = static_cast<DesignNetView *>(m_pCurEditor->designNetView());
	connect(editorView->scene(), SIGNAL(sceneRectChanged(QRectF)), this, SLOT(showScene()));
	setScene(editorView->scene());

	setSceneRect(editorView->sceneRect());
	fitInView(sceneRect(), Qt::KeepAspectRatio);
}

void MinimapNavigator::showScene()
{
	if (scene() != NULL)
	{
		setSceneRect(scene()->sceneRect());
		fitInView(sceneRect(), Qt::KeepAspectRatio);
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
	QRectF r = sceneRect();
	painter->fillRect(QRectF(0, 0, 100, 100), Qt::white);
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
		m_pCurEditor->designNetView()->centerOn(mapToScene(event->pos()));
	scene()->update();
	QGraphicsView::mouseMoveEvent(event);
}

void MinimapNavigator::mouseReleaseEvent(QMouseEvent *event)
{
	m_bPressed = false;
	QGraphicsView::mouseReleaseEvent(event);
}

void MinimapNavigator::paintEvent(QPaintEvent * event)
{
	QGraphicsView::paintEvent(event);
	QRectF rc = this->rect();
	QPainter painter(this);
	painter.drawRect(rc);
}

}
