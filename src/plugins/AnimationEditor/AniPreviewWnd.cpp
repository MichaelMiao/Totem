#include "stdafx.h"
#include "AniPreviewWnd.h"
#include <QPainter>
#include <QResizeEvent>
#include <QScrollBar>
#include "GraphicsUI/graphicsautoshowhideitem.h"
#include "AnimantionMgr.h"


#define RULER_WIDTH		20
AniPreviewScene::AniPreviewScene(QObject *parent) : QGraphicsScene(parent)
{
	m_pObject = new GraphicsUI::GraphicsAutoShowHideItem();
	m_pObject->setAutoHide(true);
	QPixmap pixmap(":/AnimationEditor/Resources/obj.jpg");
	QSize sz = pixmap.size();
	m_pObject->setPixmap(pixmap);
	m_pObject->setSize(sz);
	QMatrix matrix;
	matrix.translate(-sz.width() / 2, -sz.height() / 2);
	m_pObject->setMatrix(matrix);
	m_pObject->animateShow(true);
	addItem(m_pObject);
	m_pObject->setPos(QPointF(0, 0));
	addItem(&m_lineHori);
	addItem(&m_lineVer);
}

void AniPreviewScene::drawBackground(QPainter *painter, const QRectF &rect)
{
	painter->fillRect(rect, Qt::darkGray);
	painter->drawLine(0, rect.top(), 0, rect.bottom());
	painter->drawLine(rect.left(), 0, rect.right(), 0);
}

void AniPreviewScene::startAnimation(AnimationInfo* pInfo)
{
	m_animation.clear();
	{
		QPropertyAnimation* pAnimation = new QPropertyAnimation(m_pObject, "x");
		pAnimation->setStartValue(pInfo->ptPosFrom.x());
		pAnimation->setEndValue(pInfo->ptPosTo.x());
		pAnimation->setEasingCurve(pInfo->m_curvePosX);
		pAnimation->setDuration(pInfo->iDuration);
		m_animation.addAnimation(pAnimation);
	}
	
	{
		QPropertyAnimation* pAnimation = new QPropertyAnimation(m_pObject, "y");
		pAnimation->setStartValue(pInfo->ptPosFrom.y());
		pAnimation->setEndValue(pInfo->ptPosTo.y());
		pAnimation->setEasingCurve(pInfo->m_curvePosY);
		pAnimation->setDuration(pInfo->iDuration);
		m_animation.addAnimation(pAnimation);
	}
	{
		QPropertyAnimation* pAnimation = new QPropertyAnimation(m_pObject, "iconOpacity");
		pAnimation->setStartValue(pInfo->iAlphaFrom / 255.0);
		pAnimation->setEndValue(pInfo->iAlphaTo / 255.0);
		pAnimation->setEasingCurve(pInfo->m_curveAlpha);
		pAnimation->setDuration(pInfo->iDuration);
		m_animation.addAnimation(pAnimation);
	}

	{
		QPropertyAnimation* pAnimation = new QPropertyAnimation(m_pObject, "scale");
		pAnimation->setStartValue(pInfo->iScaleFrom / 100.0);
		pAnimation->setEndValue(pInfo->iScaleTo / 100.0);
		pAnimation->setEasingCurve(pInfo->m_curveScale);
		pAnimation->setDuration(pInfo->iDuration);
		m_animation.addAnimation(pAnimation);
	}
	
	m_animation.start();
}

AniPreviewWnd::AniPreviewWnd(FrameStorageStruct* pFS) : FSUtil(pFS), m_view(this)
{
	connect(m_view.verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(update()));
	connect(m_view.horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(update()));
	pFS->m_pPreviewWnd = this;
}

void AniPreviewWnd::paintEvent(QPaintEvent * event)
{
	QWidget::paintEvent(event);
	QRectF rc(rect());
	rc.setBottom(rc.top() + RULER_WIDTH);
	const int iVer = m_view.verticalScrollBar()->value();
	const int iHoriz = m_view.horizontalScrollBar()->value();
	QSize szViewport = m_view.viewport()->size();
	QPainter painter(this);
	painter.fillRect(rc, Qt::white);
	QRect rcVisible(iHoriz, iVer, szViewport.width(), szViewport.height());
	for (int i = 0; i < rc.right(); ++i)
	{
		int iValue = i + iHoriz;
		if (iValue % 20 == 0)
			painter.drawLine(rc.left() + RULER_WIDTH + i, rc.top(), rc.left() + RULER_WIDTH + i, rc.bottom());
	}

	for (int i = 0; i < rc.bottom(); ++i)
	{
		int iValue = i + iVer;
		if (iValue % 20 == 0)
			painter.drawLine(0, rc.top() + RULER_WIDTH + i, RULER_WIDTH, rc.top() + RULER_WIDTH + i);
	}
}

void AniPreviewWnd::resizeEvent(QResizeEvent * e)
{
	QSize sz = rect().size();
	m_view.setGeometry(QRect(RULER_WIDTH, RULER_WIDTH, sz.width() - RULER_WIDTH, sz.height() - RULER_WIDTH));
}

void AniPreviewWnd::onActionChanged(AnimationInfo* pInfo)
{
	AniPreviewScene* pScene = (AniPreviewScene*)m_view.scene();
	pScene->startAnimation(pInfo);
}

AniPreviewView::AniPreviewView(AniPreviewWnd* pParent) : QGraphicsView(pParent), m_pWnd(pParent)
{
	AniPreviewScene *scene = new AniPreviewScene(this);
	setScene(scene);
	scene->setSceneRect(-2000, -2000, 4000, 4000);
	
	setRenderHint(QPainter::Antialiasing);
	setCacheMode(CacheBackground);
	setViewportUpdateMode(BoundingRectViewportUpdate);
	setRenderHint(QPainter::Antialiasing);
	setTransformationAnchor(AnchorUnderMouse);
	centerOn(QPoint(0, 0));
}

AniPreviewView::~AniPreviewView()
{

}

void AniPreviewView::zoomIn()
{
	scaleView(qreal(1.2));
}

void AniPreviewView::zoomOut()
{
	scaleView(1 / qreal(1.2));
}

void AniPreviewView::scaleView(qreal scaleFactor)
{
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
}

void AniPreviewView::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, -event->delta() / 360.0));
}
