#include "grabcutfrontwidget.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFutureInterface>
#include <QFutureWatcher>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMetaType>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>
#include <QSlider>
#include <QUrl>
#include <QWheelEvent>
#include "../../designnet_core/designnetbase/designnetspace.h"
#include "../../designnet_core/designnetbase/processor.h"
#include "../src/corelib/concurrent/qtconcurrentresultstore.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Utils/opencvhelper.h"
#include "Utils/runextensions.h"
#include "GrabCutProcessor.h"


using namespace DesignNet;

//////////////////////////////////////////////////////////////////////////


ImageNode::ImageNode(GrabCutFrontWidget* pWidget) : m_pWidget(pWidget)
{
	setAcceptHoverEvents(true);
	m_iWidth = 2;
}

ImageNode::~ImageNode()
{

}

void ImageNode::setImage(cv::Mat src)
{
	m_srcMat = src;
	m_mask = cv::Mat::zeros(m_srcMat.rows, m_srcMat.cols, CV_8UC1);
	m_mask = cv::Scalar(cv::GC_PR_BGD);
	cv::Rect rc(m_srcMat.rows / 4, m_srcMat.cols / 4, m_srcMat.cols / 2, m_srcMat.rows / 2);
	cv::Mat roi(m_mask, rc);
	roi = cv::Scalar(cv::GC_PR_FGD);
	m_fgModel = cv::Mat();
	m_foreground = cv::Mat();
	setPixmap(QPixmap::fromImage(Utils::OpenCVHelper::Mat2QImage(m_srcMat)));
}

void ImageNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint pt = event->pos().toPoint();
		cv::Point2i p(pt.x(), pt.y());
		cv::line(m_mask, p, m_ptOld, m_pWidget->isDrawingForeground() ? cv::GC_FGD : cv::GC_BGD, m_iWidth);
		QPixmap px = pixmap();
		QPainter painter(&px);
		QColor clr = Qt::red;
		if (m_pWidget->isDrawingForeground())
			clr = Qt::yellow;
		QPen pen(clr, m_iWidth);
		painter.setPen(pen);
		painter.drawLine(QPoint(m_ptOld.x, m_ptOld.y), pt);
		setPixmap(px);
		update();
		m_ptOld.x = pt.x();
		m_ptOld.y = pt.y();
	}
	QGraphicsPixmapItem::mouseMoveEvent(event);
}

void ImageNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPoint pt = event->pos().toPoint();
		m_ptOld.x = pt.x();
		m_ptOld.y = pt.y();
		grabMouse();
	}
}

void ImageNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		ungrabMouse();
		cv::Rect rc(0, 0, m_srcMat.rows, m_srcMat.cols);
		cv::grabCut(m_srcMat, m_mask, rc, m_fgModel, m_bgModel, 1, cv::GC_INIT_WITH_MASK);
		cv::Mat matOut;
		cv::Mat matFore;
		cv::compare(m_mask, cv::GC_PR_FGD, matOut, cv::CMP_EQ) ;
		cv::compare(m_mask, cv::GC_FGD, matFore, cv::CMP_EQ) ;  
		m_foreground = cv::Mat(m_mask.size(), CV_8UC3, cv::Scalar(0));
		m_srcMat.copyTo(m_foreground, matOut);
		m_srcMat.copyTo(m_foreground, matFore);
		QImage img = Utils::OpenCVHelper::Mat2QImage(m_srcMat);
		img = img.convertToFormat(QImage::Format_ARGB32);
		for(int y = 0; y < img.height(); y++)
		{
			for(int x = 0;x < img.width(); x++)
			{
				QRgb rgb = img.pixel(x, y);
				img.setPixel(x, y, qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), (matFore.at<uchar>(y, x) == 0 && matOut.at<uchar>(y, x) == 0) ? 0 : 255));
			}
		}
		setPixmap(QPixmap::fromImage(img));
	}
}

//////////////////////////////////////////////////////////////////////////

GrabCutLabel::GrabCutLabel(GrabCutFrontWidget *pParent) : QGraphicsView(pParent), m_Item(pParent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
	setAcceptDrops(true);
	setAutoFillBackground(true);
	setBackgroundRole(QPalette::Dark);
	setTransformationAnchor(AnchorUnderMouse);

	QGraphicsScene *pScene = new QGraphicsScene(this);
	setScene(pScene);
	pScene->addItem(&m_Item);
}

void GrabCutLabel::wheelEvent(QWheelEvent *event)
{
	scaleView(pow((double)2, -event->delta() / 240.0));
}

void GrabCutLabel::dragEnterEvent(QDragEnterEvent *event)
{
	update();
	event->acceptProposedAction();
}

void GrabCutLabel::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
}

void GrabCutLabel::dragLeaveEvent(QDragLeaveEvent *event)
{
	update();
}

void GrabCutLabel::dropEvent(QDropEvent *e)
{
	const QMimeData *mimeData = e->mimeData();

	if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();
		QString text;
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			if (urlList.at(i).isLocalFile())
			{
				text = urlList.at(i).toLocalFile();
				m_srcMat = cv::imread(text.toLocal8Bit().data());
				m_Item.setImage(m_srcMat);
				break;
			}
		}
	}
	update();
}

QSize GrabCutLabel::sizeHint() const
{
	return QSize(500, 500);
}

void GrabCutLabel::scaleView(qreal scaleFactor)
{
	qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (factor < 0.07 || factor > 100)
		return;

	scale(scaleFactor, scaleFactor);
}

//////////////////////////////////////////////////////////////////////////

GrabCutFrontWidget::GrabCutFrontWidget(Processor* processor, QWidget *parent)
	: ProcessorFrontWidget(processor), m_label(this)
{
	ui.setupUi(this);
	ui.horizontalLayout->addWidget(&m_label);
	ui.radioForeground->setChecked(true);
	connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
	connect(ui.cbBackground, SIGNAL(stateChanged(int)), this, SLOT(onCheckChanged()));
	connect(ui.cbForeground, SIGNAL(stateChanged(int)), this, SLOT(onCheckChanged()));
	connect(ui.btnGo, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
}

GrabCutFrontWidget::~GrabCutFrontWidget()
{

}

void GrabCutFrontWidget::onValueChanged()
{
	m_label.getNode().setWidth(ui.horizontalSlider->value());
}

bool GrabCutFrontWidget::isDrawingForeground()
{
	return ui.radioForeground->isChecked();
}

bool GrabCutFrontWidget::isShowForeground()
{
	return ui.cbForeground->isChecked();
}

bool GrabCutFrontWidget::isShowBackground()
{
	return ui.cbBackground->isChecked();
}

void GrabCutFrontWidget::onCheckChanged()
{
	m_label.getNode().update();
}

void GrabCutFrontWidget::onBtnClicked()
{
	GrabCutProcessor* p = (GrabCutProcessor*)getProcessor();
	bool bRet = p->space()->prepareProcess();
	if (bRet == false || m_label.getMat().empty())
		return;

	p->setOutputValue(m_label.getNode().getForeground());
	p->start();
}
