#pragma once
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QParallelAnimationGroup>
#include "FrameStorage.h"


namespace GraphicsUI {
class GraphicsAutoShowHideItem;
}

class AnimationInfo;
class AniPreviewScene : public QGraphicsScene
{
public:

	AniPreviewScene(QObject *parent);
	~AniPreviewScene() {}

	void drawBackground(QPainter *painter, const QRectF &rect) override;

	void startAnimation(AnimationInfo* pInfo);

private:

	GraphicsUI::GraphicsAutoShowHideItem* m_pObject;
	QParallelAnimationGroup m_animation;
	QGraphicsLineItem		m_lineHori;
	QGraphicsLineItem		m_lineVer;
};

class AniPreviewWnd;
class AniPreviewView : public QGraphicsView
{
	Q_OBJECT

public:

	AniPreviewView(AniPreviewWnd* pParent);
	~AniPreviewView();

public slots:

	void zoomIn();
	void zoomOut();

protected:
	
	void wheelEvent(QWheelEvent *event);

private:

	void scaleView(qreal scaleFactor);

	AniPreviewWnd* m_pWnd;
};

class AniPreviewWnd : public QWidget, public FSUtil
{
	Q_OBJECT

public:

	AniPreviewWnd(FrameStorageStruct* pFS);
	~AniPreviewWnd(){}


	void paintEvent(QPaintEvent * event) override;
	void resizeEvent(QResizeEvent * event) override;
	QSize sizeHint() const { return QSize(100, 300); }

public slots:

	void onActionChanged(AnimationInfo* pInfo);

private:

	AniPreviewView m_view;
};
