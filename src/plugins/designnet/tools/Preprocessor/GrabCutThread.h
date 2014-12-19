#pragma once
#include "opencv2/core/core.hpp"
#include <QThread>


struct GrabCutData
{
	cv::Mat m_srcMat;
	cv::Mat m_mask;
	cv::Mat m_foreground;
	cv::Rect m_rc;
	double	m_fDelta;
};

class GrabCutThread : public QThread
{
public:

	GrabCutThread(QObject *parent) : QThread(parent) {}
	~GrabCutThread() { }

	void startProcess(GrabCutData* pData);
	GrabCutData* data() { return m_pData; }
	void run() override;

private:

	GrabCutData* m_pData;
};
