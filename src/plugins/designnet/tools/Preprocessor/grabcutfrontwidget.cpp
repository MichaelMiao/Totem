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

class GrabCutThread : public QThread
{
public:

	GrabCutThread(QObject *parent) : QThread(parent) {}
	~GrabCutThread() { }

	void startProcess(GrabCutData* pData)
	{
		m_pData = pData;
		start();
	}

	void run() override
	{
		cv::Rect rc(0, 0, m_pData->m_srcMat.rows, m_pData->m_srcMat.cols);
		cv::Mat matBack;
		cv::Mat matFore;
		cv::grabCut(m_pData->m_srcMat, m_pData->m_mask, rc, cv::Mat(), cv::Mat(), 5, cv::GC_INIT_WITH_MASK);
		m_pData->m_foreground = cv::Mat(m_pData->m_mask.size(), CV_8UC3, cv::Scalar(0));
		cv::compare(m_pData->m_mask, cv::GC_PR_FGD, matFore, cv::CMP_EQ);
		m_pData->m_srcMat.copyTo(m_pData->m_foreground, matFore);
		cv::compare(m_pData->m_mask, cv::GC_FGD, matFore, cv::CMP_EQ) ;
		m_pData->m_srcMat.copyTo(m_pData->m_foreground, matFore);
		cv::imwrite("D:/miao1.bmp", m_pData->m_foreground);
	}

private:
	
	GrabCutData* m_pData;
};

ImageNode::ImageNode(GrabCutFrontWidget* pWidget, GrabCutData*	pData)
	: m_pWidget(pWidget), m_pData(pData)
{
	setAcceptHoverEvents(true);
	m_iWidth = 2;
}

ImageNode::~ImageNode()
{

}

void ImageNode::show(ImageNode::ShowType eType)
{
	m_eType = eType;
	cv::Mat m;
	switch(eType)
	{
	case SRC:
		m = m_pData->m_srcMat;
		break;
	case FORE:
		m = m_pData->m_foreground;
		break;
	default:
		;
	}
	if (eType == ImageNode::BIND)
	{
		QImage img = Utils::OpenCVHelper::Mat2QImage(m_pData->m_srcMat);
		img = img.convertToFormat(QImage::Format_ARGB32);

		int width = img.width();
		int height = img.height();
		uchar *line = img.bits();
		uchar *pixel = 0;
		for (int y = 0; y < height; y++)
		{
			pixel = line;
			for (int x = 0; x < width; x++)
			{
				uchar uTemp = m_pData->m_mask.at<uchar>(y, x);
				*(pixel + 3) = (uTemp == cv::GC_PR_FGD || uTemp == cv::GC_FGD) ? 255 : 125;
				pixel += 4;
			}
			line += img.bytesPerLine();
		}

		for(int y = 0; y < img.height(); y++)
		{
			for(int x = 0;x < img.width(); x++)
			{
				uchar uTemp = m_pData->m_mask.at<uchar>(y, x);
//				QRgb rgb = img.pixel(y, x);
//				img.setPixel(y, x, qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb),
//					(uTemp == cv::GC_PR_FGD || uTemp == cv::GC_FGD) ? 255 : 125));
			}
		}
		setPixmap(QPixmap::fromImage(img));
	}
	else
	{
		setPixmap(QPixmap::fromImage(Utils::OpenCVHelper::Mat2QImage(m)));
	}
}

void ImageNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		cv::Mat &mask = m_pData->m_mask;
		QPoint pt = event->pos().toPoint();
		cv::Point2i p(pt.x(), pt.y());
		cv::line(mask, p, m_ptOld, m_pWidget->isDrawingForeground() ? cv::GC_FGD : cv::GC_BGD, m_iWidth);
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
	}
}

void ImageNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_ptOld.x = 0;
		m_ptOld.y = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

GrabCutLabel::GrabCutLabel(GrabCutFrontWidget *pParent, GrabCutData* pData) : QGraphicsView(pParent),
	m_pWidget(pParent), m_Item(pParent, pData), m_pData(pData)
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
				m_pWidget->setImage(text);
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

void GrabCutLabel::refresh()
{
}

void GrabCutLabel::show(ImageNode::ShowType eType)
{
	m_Item.show(eType);
}

//////////////////////////////////////////////////////////////////////////

GrabCutFrontWidget::GrabCutFrontWidget(Processor* processor, QWidget *parent)
	: ProcessorFrontWidget(processor), m_label(this, &m_data)
{
	ui.setupUi(this);
	ui.horizontalLayout->addWidget(&m_label);
	ui.radioForeground->setChecked(true);
	connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
	connect(ui.cbBackground, SIGNAL(stateChanged(int)), this, SLOT(onCheckChanged()));
	connect(ui.cbForeground, SIGNAL(stateChanged(int)), this, SLOT(onCheckChanged()));
	connect(ui.btnFragment, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
	connect(ui.btnRecognize, SIGNAL(clicked()), this, SLOT(onBtnClicked()));

	m_pThread = new GrabCutThread(this);
	connect(m_pThread, SIGNAL(finished()), this, SLOT(onProcessFinished()));
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

void GrabCutFrontWidget::setImage(QString strPath)
{
	m_data.m_srcMat = cv::imread(strPath.toLocal8Bit().data());
	m_data.m_mask = cv::Mat(m_data.m_srcMat.rows, m_data.m_srcMat.cols, CV_8UC1, cv::Scalar(cv::GC_PR_FGD));
	m_label.show(ImageNode::SRC);
}

void GrabCutFrontWidget::onCheckChanged()
{
	m_label.getNode().update();
}

void GrabCutFrontWidget::onBtnClicked()
{
	if (sender() == ui.btnFragment)	// 分割前景
	{
		m_pThread->startProcess(&m_data);
	}
	else if (sender() == ui.btnRecognize)	// 识别
	{
// 		GrabCutProcessor* p = (GrabCutProcessor*)getProcessor();
// 		bool bRet = p->space()->prepareProcess();
// 		if (bRet == false || m_label.getMat().empty())
// 			return;
// 
// 		p->setOutputValue(m_label.m_maskMat);
// 		p->start();
	}
}

void GrabCutFrontWidget::onProcessFinished()
{
	m_label.show(ImageNode::BIND);
// 	QImage img = Utils::OpenCVHelper::Mat2QImage(m_srcMat);
// 	img = img.convertToFormat(QImage::Format_ARGB32);
// 	for(int y = 0; y < img.height(); y++)
// 	{
// 		for(int x = 0;x < img.width(); x++)
// 		{
// 			QRgb rgb = img.pixel(x, y);
// 			img.setPixel(x, y, qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), (matFore.at<uchar>(y, x) == 0 && matOut.at<uchar>(y, x) == 0) ? 0 : 255));
// 		}
// 	}
}
