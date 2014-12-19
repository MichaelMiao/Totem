#include "GrabCutThread.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


void GrabCutThread::startProcess(GrabCutData* pData)
{
	m_pData = pData;
	start();
}

void GrabCutThread::run()
{
	cv::Rect rc(0, 0, m_pData->m_srcMat.rows, m_pData->m_srcMat.cols);
	cv::Mat matBack;
	cv::Mat matFore;
	cv::grabCut(m_pData->m_srcMat, m_pData->m_mask, rc, cv::Mat(), cv::Mat(), 1, cv::GC_INIT_WITH_MASK);
	m_pData->m_foreground = cv::Mat(m_pData->m_mask.size(), CV_8UC3, cv::Scalar(0));
	cv::compare(m_pData->m_mask, cv::GC_PR_FGD, matFore, cv::CMP_EQ);
	m_pData->m_srcMat.copyTo(m_pData->m_foreground, matFore);
	cv::compare(m_pData->m_mask, cv::GC_FGD, matFore, cv::CMP_EQ) ;
	m_pData->m_srcMat.copyTo(m_pData->m_foreground, matFore);
}
