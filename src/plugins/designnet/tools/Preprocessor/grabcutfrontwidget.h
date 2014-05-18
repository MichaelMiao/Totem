#pragma once
#ifndef GRABCUTFRONTWIDGET_H
#define GRABCUTFRONTWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include "../../../../designnet/designnet_core/widgets/processorfrontwidget.h"
#include "opencv2/core/core.hpp"
#include "ui_grabcutfrontwidget.h"

namespace DesignNet
{
class Processor;
}

class GrabCutFrontWidget;
class GrabCutLabel;
struct GrabCutData
{
	cv::Mat m_srcMat;
	cv::Mat m_mask;
	cv::Mat m_foreground;
	cv::Rect m_rc;
};

class ImageNode : public QGraphicsPixmapItem
{

public:
	
	enum ShowType{
		SRC,
		FORE,
		BACK,
		BIND,	// «∞æ∞”Î±≥æ∞ªÏ∫œ
	};

	ImageNode(GrabCutFrontWidget* pWidget, GrabCutData*	pData);
	~ImageNode();

	void setWidth(const int iWidth) { m_iWidth = iWidth; }
	void show(ImageNode::ShowType eType);

protected:

	void	mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void	mousePressEvent(QGraphicsSceneMouseEvent* event);
	void	mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:

	int			m_iWidth;

	cv::Point2i m_ptOld;

	GrabCutFrontWidget* m_pWidget;
	GrabCutData*		m_pData;
	ImageNode::ShowType	m_eType;
};

class GrabCutLabel : public QGraphicsView
{
	Q_OBJECT
public:

	
	GrabCutLabel(GrabCutFrontWidget *pParent, GrabCutData* pData);
	~GrabCutLabel() {}

	ImageNode &getNode() { return m_Item; }

	void refresh();

	void show(ImageNode::ShowType eType);

protected:

	void wheelEvent(QWheelEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

	QSize sizeHint() const;
	void scaleView(qreal scaleFactor);

private:

	QImage			m_srcImage;
	ImageNode		m_Item;
	GrabCutData*	m_pData;
	GrabCutFrontWidget* m_pWidget;
};

class GrabCutThread;
class GrabCutFrontWidget : public DesignNet::ProcessorFrontWidget
{
	Q_OBJECT

public:

	GrabCutFrontWidget(DesignNet::Processor* processor, QWidget *parent = 0);
	~GrabCutFrontWidget();

	bool isDrawingForeground();
	bool isShowForeground();
	bool isShowBackground();

	void setImage(QString strPath);

public slots:

	void onValueChanged();
	void onCheckChanged();
	void onBtnClicked();

	void onProcessFinished();

private:

	Ui_Form	ui;
	GrabCutLabel		m_label;
	GrabCutThread*		m_pThread;
	GrabCutData			m_data;
};

#endif // GRABCUTFRONTWIDGET_H
