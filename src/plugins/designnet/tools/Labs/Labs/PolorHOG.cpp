#include "StdAfx.h"
#include "PolorHOG.h"


PolorHOG::PolorHOG(void)
{
	m_iBinCount		= 9;
	m_iRingCount	= 10;
}


PolorHOG::~PolorHOG(void)
{
}

cv::Mat PolorHOG::extractFeature(cv::Mat &img)
{
	cv::Point center = getCentroid(img);
	return cv::Mat();
}

cv::Point PolorHOG::getCentroid(cv::Mat &img)
{
	return cv::Point();
}
