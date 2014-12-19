#include "GrabcutServerWrapper.h"
#include <QDebug>
#include <QTcpSocket>
#include <QProcess>
#include "../../designnet_core/designnetbase/processor.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Utils/opencvhelper.h"
#include "GrabCutProcessor.h"
#include "../../designnet_core/designnetbase/designnetspace.h"
#include "../../designnet_core/data/customdata.h"
#include "../src/corelib/kernel/qcoreapplication.h"

#define CONST_HEIGHT	240
#define CONST_WIDTH		320

static const char flag_type_recongization[] = "rec";
static const char flag_type_fragment[]      = "frag";
static const char flag_type_ip[]			= "ip";
static const char chId[] = "svm_result";
using namespace DesignNet;

static double scaleImage(cv::Mat &mat, cv::Size sz)
{
	double fScale	= qMin(sz.height * 1.0 / mat.rows, sz.width * 1.0 / mat.cols);
	cv::resize(mat, mat, cv::Size(mat.cols * fScale, mat.rows * fScale));
	return fScale;
}

GrabcutServerWrapper::GrabcutServerWrapper(DesignNet::Processor* processor) : m_processor(processor)
{
	connect(&m_server, SIGNAL(newConnection()), this, SLOT(onNewComing()));
}

GrabcutServerWrapper::~GrabcutServerWrapper()
{

}

bool GrabcutServerWrapper::startServer(QString strIP)
{
	m_strIP = strIP;
	QProcess proc;
	QStringList arguments;
	arguments << "-e " << m_strIP;
	QString str = QCoreApplication::applicationDirPath() + "/webapp/QtWebApp.exe";
	proc.start(str, arguments);
	return m_server.listen(QHostAddress::Any, 8001);
}

void GrabcutServerWrapper::onNewComing()
{
	QTcpSocket *tcpSocket = m_server.nextPendingConnection();
	m_vecSocket.push_back(tcpSocket);
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(onReadMessage())); //有可读的信息
}

void GrabcutServerWrapper::onReadMessage()
{
	QTcpSocket* pSocket = (QTcpSocket*)sender();
	QByteArray dataIn = pSocket->readAll(); //读取  
	QDataStream s(dataIn);
	int iId;
	QString typeName, strSrcFileName, strMaskFileName;
	QRect rc(0, 0, 0, 0);
	s >> rc >>typeName;
	if (typeName == QLatin1String(flag_type_fragment))
	{
		s >> iId >> strSrcFileName >> strMaskFileName;
		
		doFragment(iId, rc, strSrcFileName, strMaskFileName, pSocket);
	}
	else if (typeName == QLatin1String(flag_type_ip))
	{
		QByteArray byteData;
		QDataStream s(&byteData, QIODevice::ReadWrite);
		s.setVersion(QDataStream::Qt_4_0);
		s << m_strIP;
		if (pSocket->isValid())
			pSocket->write(byteData);
		pSocket->waitForBytesWritten();
	}
	else
	{
		s >> iId;
		doRecognization(iId, pSocket);
	}
}

void GrabcutServerWrapper::onProcessFinished()
{
	GrabCutThread* pThread = (GrabCutThread*)sender();
	GrabCutData* pData = pThread->data();
	GrabCutThreadData* pThreadData = (GrabCutThreadData*)pThread->userData(0);
	{
		QString strResult = tr("D:/temp/%1_result.png").arg(pThreadData->m_iId);
		QString strForeground = tr("D:/temp/%1_fore.png").arg(pThreadData->m_iId);
		QString strBackground = tr("D:/temp/%1_back.png").arg(pThreadData->m_iId);
		//////////////////////////////////////////////////////////////////////////
		// 原图弄成半透明
		QImage imgSrc = Utils::OpenCVHelper::Mat2QImage(pData->m_srcMat);
		imgSrc = imgSrc.convertToFormat(QImage::Format_ARGB32);
		int width = imgSrc.width();
		int height = imgSrc.height();
		uchar *line = imgSrc.bits();
		uchar *pixel = 0;
		for (int y = 0; y < height; y++)
		{
			pixel = line;
			for (int x = 0; x < width; x++)
			{
				uchar uTemp = pData->m_mask.at<uchar>(y, x);
				*(pixel + 3) = (uTemp == cv::GC_PR_FGD || uTemp == cv::GC_FGD) ? 255 : 125;
				pixel += 4;
			}
			line += imgSrc.bytesPerLine();
		}
		imgSrc.save(strResult);
		//////////////////////////////////////////////////////////////////////////
		// 背景图
		cv::Mat matBack = pData->m_srcMat.clone();
		for (int i = 0; i < pData->m_foreground.rows; i++)
		{
			uchar* pLine = matBack.ptr<uchar>(i);
			uchar* pLineFore = pData->m_foreground.ptr<uchar>(i);
			for (int j = 0; j < pData->m_foreground.cols; j++)
			{
				int iPos = j * 3;
				if (!(pLineFore[iPos] == 0 && pLineFore[iPos + 1] == 0 && pLineFore[iPos + 2] == 0))
				{
					pLine[iPos] = pLine[iPos + 1] = pLine[iPos + 2] = 0;
				}
			}
		}
		QImage img = Utils::OpenCVHelper::Mat2QImage(pData->m_foreground);
		img.convertToFormat(QImage::Format_ARGB32);
		img.save(strForeground);
		img = Utils::OpenCVHelper::Mat2QImage(matBack);
		img.convertToFormat(QImage::Format_ARGB32);
		img.save(strBackground);
		QByteArray byteData;
		QDataStream s(&byteData, QIODevice::WriteOnly);
		s << pThreadData->m_iId
			<< tr("%1_result.png").arg(pThreadData->m_iId)
			<< tr("%1_fore.png").arg(pThreadData->m_iId)
			<< tr("%1_back.png").arg(pThreadData->m_iId);
		pThreadData->m_pSocket->write(byteData);
	}
	pThread->quit();
	delete pData;
	delete pThreadData;
}

void GrabcutServerWrapper::doFragment(int iId, QRect rc, QString strSrcFileName, QString strMaskFileName, QTcpSocket* pSocket)
{
	GrabCutThread*		pThread = new GrabCutThread(this);
	GrabCutThreadData*	pThreadData = new GrabCutThreadData;
	cv::Mat matSrc		= cv::imread(strSrcFileName.toLocal8Bit().data());
	cv::Mat matMaskPng	= cv::imread(strMaskFileName.toLocal8Bit().data());
	cv::imwrite("G:/matSrc_old.png", matSrc);
	cv::imwrite("G:/maskpng_old.png", matMaskPng);

	double fScale = scaleImage(matSrc, cv::Size(CONST_WIDTH, CONST_HEIGHT));
	scaleImage(matMaskPng, cv::Size(CONST_WIDTH, CONST_HEIGHT));
	cv::Mat matMask(matSrc.rows, matSrc.cols, CV_8UC1);
	matMask.setTo(cv::GC_BGD);
	cv::Rect rcFore(qMax((int)(rc.left() * fScale), 0), qMax((int)(rc.top() * fScale), 0), 
		qMin((int)(rc.width() * fScale), matMask.cols - 10), qMin((int)(rc.height() * fScale), matMask.rows - 10));
	int iW = rcFore.width;
	int iH = rcFore.height;
	int iLeft = rcFore.tl().x;
	int itop = rcFore.tl().y;
	matMask(rcFore).setTo(cv::GC_PR_FGD);
	for (int i = 0; i < matMaskPng.rows; i++)
	{
		uchar* pLine = matMaskPng.ptr<uchar>(i);
		for(int j = 0; j < matMaskPng.cols; j++)
		{
			int iPos = j * 3;
			if (pLine[iPos] == 255)
				matMask.at<uchar>(i, j) = cv::GC_BGD;
			else if (pLine[iPos + 2] == 255)
				matMask.at<uchar>(i, j) = cv::GC_FGD;
		}
	}
	cv::imwrite("G:/matSrc.png", matSrc);
	cv::imwrite("G:/maskpng.png", matMaskPng);
	cv::imwrite("G:/mask.png", matMask);

	pThreadData->m_iId = iId;
	pThreadData->m_pSocket = pSocket;
	pThreadData->m_pThread = pThread;
	pThread->setUserData(0, (QObjectUserData*)pThreadData);

	GrabCutData* pData = new GrabCutData;
	pData->m_srcMat		= matSrc;
	pData->m_mask		= matMask;
//	pData->m_fDelta		= f;
	connect(pThread, SIGNAL(finished()), this, SLOT(onProcessFinished()));
	pThread->startProcess(pData);
}

void GrabcutServerWrapper::doRecognization(int iId, QTcpSocket* pSocket)
{
	RecognizeTaskData data;
	data.iId		= iId;
	data.m_pSocket	= pSocket;
	m_vecTask.push_back(data);
	GrabCutProcessor* p = (GrabCutProcessor*)m_processor;
	bool bRet = p->space()->prepareProcess();
	if (bRet == false)
		return;
	QString strForeground = tr("D:/temp/%1_fore.png").arg(iId);
	cv::Mat matFore = cv::imread(strForeground.toLocal8Bit().data());
	p->setOutputValue(matFore);
	p->notifyProcess();
}

void GrabcutServerWrapper::sendResult(cv::Mat res)
{
	QByteArray byteData;
	QDataStream s(&byteData, QIODevice::ReadWrite);
	s.setVersion(QDataStream::Qt_4_0);
	for (int i = 0; i < res.cols; i++)
		s << (int)res.at<int>(0, i);

	RecognizeTaskData data = m_vecTask.front();
	if (data.m_pSocket->isValid())
		data.m_pSocket->write(byteData);
	data.m_pSocket->waitForBytesWritten();
	m_vecTask.pop_front();
}

void GrabcutServerWrapper::onObjectAdded(QObject *obj)
{
	if (m_vecTask.size() > 0)
	{
		CustomData* pData = qobject_cast<CustomData*>(obj);
		if (pData)
		{
			if (pData->id().toString() == QLatin1String(chId))
			{
				cv::Mat* res = (cv::Mat*)pData->data();
				sendResult(*res);
			}
		}
	}
}
