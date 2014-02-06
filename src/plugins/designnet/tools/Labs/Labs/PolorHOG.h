#pragma once
#include "opencv2\core\core.hpp"


class PolorHOG
{
public:
	PolorHOG(void);
	~PolorHOG(void);
	cv::Mat extractFeature(cv::Mat &img);

	cv::Point getCentroid(cv::Mat &img);
private:

	int m_iRingCount;	//!< 圆环个数
	int m_iBinCount;	//!< bin的个数
};

