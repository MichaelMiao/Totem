#include "grabcutfrontwidget.h"
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFutureInterface>
#include <QFutureWatcher>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMetaType>
#include <QMouseEvent>
#include <QObject>
#include <QPainter>
#include <QPaintEvent>
#include <QSizePolicy>
#include <QSlider>
#include <QSplitter>
#include <QUrl>
#include <QWheelEvent>
#include "../../designnet_core/data/customdata.h"
#include "../../designnet_core/designnetbase/designnetspace.h"
#include "../../designnet_core/designnetbase/processor.h"
#include "../src/corelib/concurrent/qtconcurrentresultstore.h"
#include "extensionsystem/pluginmanager.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Utils/opencvhelper.h"
#include "Utils/runextensions.h"
#include "GrabCutProcessor.h"


using namespace DesignNet;

//////////////////////////////////////////////////////////////////////////

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
		GrabCutFrontWidget::DrawType dt = m_pWidget->getDrawType();
		
		switch(dt)
		{
		case GrabCutFrontWidget::DrawType_Fore:
		case GrabCutFrontWidget::DrawType_Back:
			{
				QPixmap px = pixmap();
				QPainter painter(&px);
				cv::Mat &mask = m_pData->m_mask;
				QPoint pt = event->pos().toPoint();
				cv::Point2i p(pt.x(), pt.y());

				cv::line(mask, p, m_ptOld, dt == GrabCutFrontWidget::DrawType_Fore ? cv::GC_FGD : cv::GC_BGD, m_iWidth);
				
				QColor clr = Qt::red;
				if (GrabCutFrontWidget::DrawType_Fore == dt)
					clr = Qt::yellow;
				QPen pen(clr, m_iWidth);
				painter.setPen(pen);
				painter.drawLine(QPoint(m_ptOld.x, m_ptOld.y), pt);
				setPixmap(px);
				update();
				m_ptOld.x = pt.x();
				m_ptOld.y = pt.y();
			}
			break;
		case GrabCutFrontWidget::DrawType_Rect:
			{
				show(SRC);
				QPixmap px = pixmap();
				QPainter painter(&px);
				if (m_ptOld.x < 0 || m_ptOld.y < 0)
					break;
				QPoint pt = event->pos().toPoint();
				pt.setX(qMax(qMin(pt.x(), px.width()), 0));
				pt.setY(qMax(qMin(pt.y(), px.height()), 0));
				cv::Point2i p(pt.x(), pt.y());

				m_pData->m_rc = cv::Rect(m_ptOld, p);
				QColor clr = Qt::green;
				QPen pen(clr, 3);
				painter.setPen(pen);
				painter.drawRect(QRect(m_pData->m_rc.tl().x, m_pData->m_rc.tl().y, m_pData->m_rc.width, m_pData->m_rc.height));
				setPixmap(px);
				update();
			}
			break;
		}
		
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
		m_ptOld.x = -1;
		m_ptOld.y = -1;
		if (m_pWidget->getDrawType() == GrabCutFrontWidget::DrawType_Rect)
		{
			if (m_pData->m_rc.area() > 10)
			{
				m_pData->m_mask.setTo(cv::GC_BGD);
				m_pData->m_mask(m_pData->m_rc).setTo(cv::GC_PR_FGD);
			}
		}
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
	: ProcessorFrontWidget(processor), m_label(this, &m_data), m_wrapper(processor)
{
	ui.setupUi(this);
	QSplitter* pSplitter = new QSplitter(this);
	pSplitter->addWidget(&m_label);
	pSplitter->addWidget(ui.widget);
	layout()->addWidget(pSplitter);
	ui.radioForeground->setChecked(true);
	connect(ui.horizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
	connect(ui.btnFragment, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
	connect(ui.btnRecognize, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
	connect(ui.btnStartServer, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
	connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject *)),
		&m_wrapper, SLOT(onObjectAdded(QObject *)));
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

GrabCutFrontWidget::DrawType GrabCutFrontWidget::getDrawType() const
{
	QAbstractButton* pBtn = ui.radioForeground->group()->checkedButton();
	if (pBtn == ui.radioRect)
		return DrawType_Rect;
	else if (pBtn == ui.radioForeground)
		return DrawType_Fore;
	else
		return DrawType_Back;
}

static double scaleImage(cv::Mat &mat, cv::Size sz)
{
	double fScale	= qMin(sz.height * 1.0 / mat.rows, sz.width * 1.0 / mat.cols);
	cv::resize(mat, mat, cv::Size(mat.cols * fScale, mat.rows * fScale));
	return fScale;
}

void GrabCutFrontWidget::setImage(QString strPath)
{
	m_data.m_srcMat = cv::imread(strPath.toLocal8Bit().data());
	scaleImage(m_data.m_srcMat, cv::Size(320, 240));
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
		GrabCutProcessor* p = (GrabCutProcessor*)getProcessor();
		bool bRet = p->space()->prepareProcess();
		if (bRet == false)
			return;

		p->setOutputValue(m_data.m_foreground);
		p->notifyProcess();
	}
	else if (sender() == ui.btnStartServer)
	{
		if (ui.lineEdit->text().length() == 0)
			QMessageBox::warning(this, tr("Failed"), tr("IP can't be empty"));

		bool bRet = m_wrapper.startServer(ui.lineEdit->text());
		if (!bRet)
			QMessageBox::warning(this, tr("Failed"), tr("start server faild"));
	}
}

void GrabCutFrontWidget::onProcessFinished()
{
	m_label.show(ImageNode::BIND);
	static int i = 159;
	char chp[255];
	sprintf(chp, "G:/%d.bmp", i);
	cv::imwrite(chp, m_data.m_foreground);
	i++;
}