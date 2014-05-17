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

class ImageNode : public QGraphicsPixmapItem
{

public:

	ImageNode(GrabCutFrontWidget* pWidget);
	~ImageNode();

	void setImage(cv::Mat src);
	void setWidth(const int iWidth) { m_iWidth = iWidth; }

	cv::Mat getForeground() { return m_foreground; }

protected:

	void	mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	void	mousePressEvent(QGraphicsSceneMouseEvent* event);
	void	mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

private:

	cv::Mat		m_srcMat;
	cv::Mat		m_mask;
	cv::Mat		m_fgModel;
	cv::Mat		m_bgModel;
	cv::Mat		m_foreground;
	int			m_iWidth;

	cv::Point2i m_ptOld;

	GrabCutFrontWidget* m_pWidget;
};

class GrabCutLabel : public QGraphicsView
{
	Q_OBJECT
public:

	GrabCutLabel(GrabCutFrontWidget *pParent);
	~GrabCutLabel() {}

	ImageNode &getNode() { return m_Item; }
	cv::Mat getMat() { return m_srcMat; }

protected:

	void wheelEvent(QWheelEvent *event) override;

	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dragLeaveEvent(QDragLeaveEvent *event) override;
	void dropEvent(QDropEvent *event) override;

	QSize sizeHint() const;
	void scaleView(qreal scaleFactor);
private:

	cv::Mat m_srcMat;
	QImage	m_srcImage;
	ImageNode m_Item;
};


class GrabCutFrontWidget : public DesignNet::ProcessorFrontWidget
{
	Q_OBJECT

public:

	GrabCutFrontWidget(DesignNet::Processor* processor, QWidget *parent = 0);
	~GrabCutFrontWidget();

	bool isDrawingForeground();
	bool isShowForeground();
	bool isShowBackground();

public slots:

	void onValueChanged();
	void onCheckChanged();
	void onBtnClicked();

private:

	Ui_Form	ui;
	GrabCutLabel m_label;
};

#endif // GRABCUTFRONTWIDGET_H
