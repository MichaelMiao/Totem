#include "stdafx.h"
#include "FlowerFeatureExtractor.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <qglobal.h>
#include <fstream>
#include <QDebug>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#define PI	3.1415926

using namespace cv;

static int compareDis(std::pair<cv::Point2i, float> p1, std::pair<cv::Point2i, float> p2)
{
	return p1.second > p2.second;
}

FlowerFeatureExtractor::FlowerFeatureExtractor(cv::Mat matBGR)
{
	m_srcMat = matBGR;
	m_glcm = cv::Mat::zeros(8, 8, CV_32F);
	m_glcm_sobel = cv::Mat::zeros(8, 8, CV_32F);
	cv::cvtColor(m_srcMat, m_hsvMat, COLOR_BGR2HSV);
	cv::cvtColor(m_srcMat, m_grayMat, COLOR_BGR2GRAY);
	cv::GaussianBlur(m_binaryMat, m_binaryMat, cv::Size(3, 3), 0);
	cv::morphologyEx(m_binaryMat, m_binaryMat, cv::MORPH_CLOSE, cv::Mat(5,5,CV_8U,Scalar(1)));
	cv::threshold(m_grayMat, m_binaryMat, 1, 255, CV_THRESH_BINARY);
}

cv::Mat FlowerFeatureExtractor::extractFeature(cv::Mat mat)
{
	m_srcMat = mat;
	m_glcm = cv::Mat::zeros(8, 8, CV_32F);
	m_glcm_sobel = cv::Mat::zeros(8, 8, CV_32F);
	cv::cvtColor(m_srcMat, m_hsvMat, COLOR_BGR2HSV);
	cv::cvtColor(mat, m_grayMat, COLOR_BGR2GRAY);
	cv::GaussianBlur(m_binaryMat, m_binaryMat, cv::Size(3, 3), 0);
	cv::morphologyEx(m_binaryMat, m_binaryMat, cv::MORPH_CLOSE, cv::Mat(5,5,CV_8U,Scalar(1)));
	cv::threshold(m_grayMat, m_binaryMat, 1, 255, CV_THRESH_BINARY);

	/// 求中心
	cv::Point2d centroid(0, 0);
	vector<vector<cv::Point> > countours;

	cv::findContours(m_binaryMat.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
	double maxarea = 0;
	cv::Rect maxrect(0, 0, 2, 2);
	size_t max_index = -1;
	for (size_t i = 0; i < counts ; ++i)
	{
		cv::Rect rect = cv::boundingRect(countours.at(i));
		if(rect.area() > maxarea)
		{
			maxarea = rect.area();
			maxrect = rect;
			max_index = i;
		}
	}
	m_rect = maxrect;
	/// 求重心 坐标求和后求平均值
	int iCount = 0;
	int iX = 0;
	int iY = 0;
	int iBottom = maxrect.y + maxrect.height;
	int iRight	= maxrect.x + maxrect.width;
	int iLeft	= maxrect.x;
	int iTop	= maxrect.y;
	for(int r = maxrect.y; r <= iBottom; ++r)
	{
		for(int c = maxrect.x; c <= iRight; ++c)
		{
			if (m_binaryMat.at<uchar>(r, c) == 255)
			{
				iX += c;
				iY += r;
				iCount++;
			}
		}
	}
	centroid.x = iX * 1.0 / iCount;
	centroid.y = iY * 1.0 / iCount;
	m_centroid = centroid;
	//////////////////////////////////////////////////////////////////////////
	double radius = qMax(qMax(iRight - centroid.x, centroid.x - iLeft), qMax(centroid.y - iTop, iBottom - centroid.y));/// 半径
	double B_min = radius / 3;
	double B_max = radius * 2 / 3;
	double A_min = radius * 5 / 6;
	double A_max = radius;
	cv::Mat matRingA = m_binaryMat.clone();
	for (int i = 0; i < matRingA.rows; ++i)
	{
		for(int j = 0; j < matRingA.cols; ++j)
		{
			double d = sqrt((double)(i - centroid.y) * (i - centroid.y) + (j - centroid.x) * (j - centroid.x));
			if (!(d >= A_min && d <= A_max))
				matRingA.at<uchar>(i, j) = 0;
		}
	}

	cv::Mat matRingB = cv::Mat::zeros(matRingA.rows, matRingA.cols, CV_8UC1);
	for (int i = 0; i < matRingB.rows; ++i)
	{
		for(int j = 0; j < matRingB.cols; ++j)
		{
			double d = sqrt((double)(i - centroid.y) * (i - centroid.y) + (j - centroid.x) * (j - centroid.x));
			if (d >= B_min && d <= B_max)
				matRingB.at<uchar>(i, j) = 255;
		}
	}
	
// 	/// 花瓣个数
// 	vector<vector<cv::Point> > countoursExternal;
// 	cv::findContours(matRingA.clone(), countoursExternal, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
// 
// 	/// 花瓣之间的角度
// 	double fAngle = 0;
// 	std::vector<double> fVecAngle;
// 	for (size_t t = 0; t < countoursExternal.size(); t++)
// 	{
// 		double fTemp = 0;
// 		cv::Rect rc = cv::boundingRect(countoursExternal.at(t));
// 		int iX = rc.x + rc.width / 2;
// 		int iY = rc.y + rc.height / 2;
// 		if (iX == centroid.x)
// 		{
// 			fTemp = iY < centroid.y ? PI / 2 : (-PI / 2);
// 		}
// 		else
// 		{
// 			fTemp = atan((centroid.y - iY) / (iX - centroid.x));
// 			if (iX < centroid.x)
// 				fTemp += PI;
// 			else if (iX > centroid.x && iY > centroid.y)
// 				fTemp += PI * 2;
// 		}
// 		fVecAngle.push_back(fTemp);
// 	}
// 	for (size_t t = 0; t < fVecAngle.size(); t++)
// 		fAngle += qAbs(fVecAngle[t] - fVecAngle[(t + 1) % fVecAngle.size()]);
// 	
// 	fAngle /= countoursExternal.size();

	cv::Mat fGLCM = extractGLCM();
	fGLCM = fGLCM.reshape(1, fGLCM.rows * fGLCM.cols);
	cv::Mat fColor = extractColor();
	fColor = fColor.reshape(1, fColor.rows * fColor.cols);
	cv::Mat fShape = getGradient();
	fShape = fShape.reshape(1, fShape.rows * fShape.cols);
	cv::Mat ret;
	ret.push_back(fGLCM);
	ret.push_back(fColor);
	ret.push_back(fShape);
	ret = ret.reshape(1, 1);
	return ret;
}

float FlowerFeatureExtractor::energe(const cv::Mat &glcm)
{
	float sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
			sum += glcm.at<float>(i,j) * glcm.at<float>(i,j);
	}
	return sum;
}

float FlowerFeatureExtractor::contrast(const cv::Mat &glcm)
{
	double sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
			sum += glcm.at<float>(i,j) * (i - j) * (i - j);
	}
	return sum;
}

float FlowerFeatureExtractor::homogeneity(const cv::Mat &glcm)
{
	float sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
			sum += glcm.at<float>(i,j) / (qAbs(i - j)  + 1);
	}
	return sum;
}

void FlowerFeatureExtractor::normalize(cv::Mat &glcm)
{
	float sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			sum += glcm.at<float>(i,j);
		}
	}
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			glcm.at<float>(i,j) = glcm.at<float>(i,j) / sum;
		}
	}
}

cv::Mat FlowerFeatureExtractor::siftFeature(cv::Mat mat)
{
	return cv::Mat();
}

cv::Mat FlowerFeatureExtractor::extractShape()
{
	/// 求中心
	cv::Point2d centroid(0, 0);
	vector<vector<cv::Point> > countours;
	cv::findContours(m_binaryMat.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
	double maxarea = 0;
	cv::Rect maxrect(0, 0, 2, 2);
	size_t max_index = -1;
	for (size_t i = 0; i < counts ; ++i)
	{
		cv::Rect rect = cv::boundingRect(countours.at(i));
		if(rect.area() > maxarea)
		{
			maxarea = rect.area();
			maxrect = rect;
			max_index = i;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	/// 求重心 坐标求和后求平均值
	int iCount = 0;
	int iX = 0;
	int iY = 0;
	int iBottom = maxrect.y + maxrect.height;
	int iRight	= maxrect.x + maxrect.width;
	int iLeft	= maxrect.x;
	int iTop	= maxrect.y;
	for(int r = maxrect.y; r <= iBottom; ++r)
	{
		for(int c = maxrect.x; c <= iRight; ++c)
		{
			if (m_binaryMat.at<uchar>(r, c) == 255)
			{
				iX += c;
				iY += r;
				iCount++;
			}
		}
	}
	centroid.x = iX * 1.0 / iCount;
	centroid.y = iY * 1.0 / iCount;

	double radius = qMax(qMax(iRight - centroid.x, centroid.x - iLeft), qMax(centroid.y - iTop, iBottom - centroid.y));/// 半径
	float fFeature[4];
	memset(fFeature, 0, sizeof(float) * 4);
	vector<cv::Point> &vecPoints = countours.at(max_index);
	std::vector<float> vecD;
	float fSum = 0;
	for (size_t t = 0; t < vecPoints.size(); t++)
	{
		cv::Point dis = vecPoints.at(t) - cv::Point(centroid.x, centroid.y);
		float f = sqrt((double)dis.dot(dis));
		fSum += f;
		vecD.push_back(f);
	}
	fSum /= vecPoints.size();
	std::vector<int> vecKey;
	if (vecD.size() >= 2)
	{
		float fTemp = vecD.at(1) - vecD.at(0);
		float f = fTemp;
		float fMax = 0, fMin = 10000;
		int iPosLast = 0;
		for (size_t t = 2; t < vecD.size() - 1; t++)
		{
			fTemp = vecD.at(t) - vecD.at(t - 1);
//			if (f < 0 && fTemp > 0 || (f > 0 && fTemp < 0) && fTemp > 0)
			if (vecD.at(t) < vecD.at(t - 1) && vecD.at(t) < vecD.at(t + 1))
			{
				if (fMax < vecD.at(t))
					fMax = vecD.at(t);
				else if (fMin > vecD.at(t))
					fMin = vecD.at(t);
				iPosLast = t;
				vecKey.push_back(t);
				f = fTemp;
			}
		}
		float fThreshold = (fMax + fMin) / 2;
		std::vector<int> goodKey;
		for (size_t t = 1; t < vecKey.size(); t++)
		{
			if (vecD.at(vecKey.at(t)) < fThreshold)
			{
				goodKey.push_back(vecKey.at(t));
			}
		}
		int iCount = goodKey.size();
		for (size_t t = 0; t < goodKey.size(); t++)
		{
			cv::Rect rc(vecPoints.at(goodKey.at(t)).x, vecPoints.at(goodKey.at(t)).y, 10, 10);
			cv::rectangle(m_srcMat, rc, cv::Scalar(255, 0, 0));
		}
	}
	
	cv::Mat mat;
	for (int i = 0; i < 4; i++)
	{
		mat.push_back(fFeature[i]);
	}
	mat.reshape(1, 1);
	return mat;
}

cv::Mat FlowerFeatureExtractor::extractShape2()
{
	return getGradient();
	cv::Moments m = cv::moments(m_binaryMat, true);
	double dhu[7];
	cv::HuMoments(m, dhu);
	cv::Mat feature(1, 7, CV_32FC1);
	for (int i = 0; i < 7; i++)
		feature.at<float>(0, i) = dhu[i];
	return feature;
}

cv::Mat FlowerFeatureExtractor::getGradient()
{
	vector<vector<cv::Point> > countours;
	cv::findContours(m_binaryMat.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double maxarea = 0;
	cv::Rect maxrect(0, 0, 2, 2);
	size_t max_index = -1;
	for (size_t i = 0; i < countours.size() ; ++i)
	{
		cv::Rect rect = cv::boundingRect(countours.at(i));
		if(rect.area() > maxarea)
		{
			maxarea = rect.area();
			maxrect = rect;
			max_index = i;
		}
	}
	const int iR = 10;
	vector<cv::Point> &max_countours = countours.at(max_index);
	std::vector<int> vecCounts;
	for (size_t t = 0; t < max_countours.size(); t++)
	{
		cv::Point p = max_countours.at(t);
		cv::Rect rc(cv::Point(p.x - iR, p.y - iR), cv::Size(2 * iR, 2 * iR));
		int iCount = 0;
		for (int r = rc.y; r < rc.y + rc.height; r++)
		{
			for (int c = rc.x; c < rc.x + rc.width; c++)
			{
				if (r < 0 || c < 0 || r >= m_binaryMat.rows || c >= m_binaryMat.cols)
					continue;

				uchar u = m_binaryMat.at<uchar>(r,c);
				int irr = sqrt(double(r - p.y) * (r - p.y) + (c - p.x) * (c - p.x));
				if (irr < iR && u == 255)
				{
					iCount++;
				}
			}
		}
		vecCounts.push_back(iCount);
	}
	int iMaxArea = PI * iR * iR;
	cv::MatND hist;
	float hranges[] = { 0, iMaxArea };
	const float *ranges[] = { hranges };
	int channels[] = { 0 };
	int hbins = 15;
	int histSize[] = { hbins };
	cv::Mat matCounts(vecCounts);
	matCounts.convertTo(matCounts, CV_32FC1);
	cv::calcHist(&matCounts, 1, channels, cv::Mat(),
		hist, 1, &hbins, ranges,
		true,
		false);
	hist = hist.reshape(1,1);
	return hist;
}

cv::Mat FlowerFeatureExtractor::extractGLCM()
{
	cv::imwrite("I:/data/binary.bmp", m_binaryMat);
	cv::imwrite("I:/data/grayMat.bmp", m_grayMat);
	/// 求中心
	cv::Point2d centroid(0, 0);
	vector<vector<cv::Point> > countours;

	cv::findContours(m_binaryMat.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
	double maxarea = 0;
	cv::Rect maxrect(0, 0, 2, 2);
	size_t max_index = -1;
	for (size_t i = 0; i < counts ; ++i)
	{
		cv::Rect rect = cv::boundingRect(countours.at(i));
		if(rect.area() > maxarea)
		{
			maxarea = rect.area();
			maxrect = rect;
			max_index = i;
		}
	}
	m_rect = maxrect;
	/// 求重心 坐标求和后求平均值
	int iCount = 0;
	int iX = 0;
	int iY = 0;
	int iBottom = maxrect.y + maxrect.height;
	int iRight	= maxrect.x + maxrect.width;
	int iLeft	= maxrect.x;
	int iTop	= maxrect.y;
	for(int r = maxrect.y; r <= iBottom; ++r)
	{
		for(int c = maxrect.x; c <= iRight; ++c)
		{
			if (m_binaryMat.at<uchar>(r, c) == 255)
			{
				iX += c;
				iY += r;
				iCount++;
			}
		}
	}
	centroid.x = iX * 1.0 / iCount;
	centroid.y = iY * 1.0 / iCount;
	m_centroid = centroid;
	//////////////////////////////////////////////////////////////////////////
	double radius = qMax(qMax(iRight - m_centroid.x, m_centroid.x - iLeft), qMax(m_centroid.y - iTop, iBottom - m_centroid.y));/// 半径
	double B_min = radius / 3;
	double B_max = radius * 2 / 3;
	double A_min = radius * 5 / 6;
	double A_max = radius;

	/// 求灰度共生矩阵(包括原灰度图的glcm和经过sobel算子滤波后的图像的glcm)
	cv::Mat sobelMat;
	cv::Sobel(m_grayMat, sobelMat, -1, 1, 0);
	{
		char ch[255];
		static int i = 0;
		sprintf(ch, "I:/data/miao/%d_test_before.xml", i++);
		cv::FileStorage fsfesture(ch, cv::FileStorage::WRITE);
		fsfesture << "grayMat" << m_grayMat;
		sprintf(ch, "I:/data/miao/%d_test_before.bmp", i++);
		cv::imwrite(ch, m_grayMat);
		fsfesture << "sobelMat" << sobelMat;
		fsfesture.release();
	}
	
	double deltaBin = 255.0 / 8; /// 每个bin的范围
	cv::Point2i p1, p2;
	for (int sita = 0; sita < 360; ++sita)
	{
		for(int r = 0; r < radius; ++r)
		{
			p1.x = centroid.x + r * cos(float(sita));
			p1.y = centroid.y - r * sin(float(sita));
			p2.x = p1.x + cos(float(sita));		/// 等价于p2.x = centroid.x + (r+1) * cos(sita);
			p2.y = p1.y - sin(float(sita));		/// 等价于p2.y = centroid.y - (r+1) * sin(sita);
			if (p2.x < 0 || p2.y < 0 || p1.x < 0 || p1.y < 0 || 
				p2.x >= m_grayMat.cols || p1.x >= m_grayMat.cols ||
				p2.y >= m_grayMat.rows || p1.y >= m_grayMat.rows)
			{
				continue;
			}
			///找到P1, p2 两个点的灰度级
			int v1 = m_grayMat.at<uchar>(p1.y, p1.x)/deltaBin;
			int v2 = m_grayMat.at<uchar>(p2.y, p2.x)/deltaBin;
			v1 = v1 >= m_glcm.rows ? m_glcm.rows - 1 : v1;
			v2 = v2 >= m_glcm.cols ? m_glcm.cols - 1 : v2;
			if (v1 < 0)
			{
				v1 = 0;
			}
			if (v2 < 0)
			{
				v2 = 0;
			}
			m_glcm.at<float>(v1, v2)++;

			v1 = sobelMat.at<uchar>(p1.y, p1.x)/deltaBin;
			uchar f = sobelMat.at<uchar>(p2.y, p2.x);
			v2 = sobelMat.at<uchar>(p2.y, p2.x)/deltaBin;
			v1 = v1 >= m_glcm.rows ? m_glcm.rows - 1 : v1;
			v2 = v2 >= m_glcm.cols ? m_glcm.cols - 1 : v2;
			if (v1 < 0)
			{
				v1 = 0;
			}
			if (v2 < 0)
			{
				v2 = 0;
			}
			m_glcm_sobel.at<float>(v1, v2)++;
		}
	}
	char ch[255];
	static int i = 0;
	sprintf(ch, "I:/data/miao/%d_test_after.xml", i++);
	cv::FileStorage fsfesture(ch, cv::FileStorage::WRITE);
	fsfesture << "m_glcm" << m_glcm;
	fsfesture << "m_glcm_sobel" << m_glcm_sobel;
	fsfesture.release();

// 	for (int sita = 0; sita < 360; ++sita)
// 	{
// 		for(int r = B_min; r < B_max; ++r)
// 		{
// 			p1.x = m_centroid.x + r * cos(float(sita));
// 			p1.y = m_centroid.y - r * sin(float(sita));
// 			p2.x = p1.x + cos(float(sita));		/// 等价于p2.x = centroid.x + (r+1) * cos(sita);
// 			p2.y = p1.y - sin(float(sita));		/// 等价于p2.y = centroid.y - (r+1) * sin(sita);
// 			if (p2.x < 0 || p2.y < 0 || p1.x < 0 || p1.y < 0 || 
// 				p2.x >= m_grayMat.cols || p1.x >= m_grayMat.cols ||
// 				p2.y >= m_grayMat.rows || p1.y >= m_grayMat.rows)
// 			{
// 				continue;
// 			}
// 			///找到P1, p2 两个点的灰度级
// 			int v1 = m_grayMat.at<uchar>(p1.y, p1.x) / deltaBin;
// 			int v2 = m_grayMat.at<uchar>(p2.y, p2.x) / deltaBin;
// 			v1 = v1 >= m_glcm.rows ? m_glcm.rows - 1 : v1;
// 			v2 = v2 >= m_glcm.cols ? m_glcm.cols - 1 : v2;
// 
// 			if (v1 < 0)
// 				v1 = 0;
// 
// 			if (v2 < 0)
// 				v2 = 0;
// 			m_glcm.at<float>(v1, v2)++;
// 
// 			v1 = sobelMat.at<uchar>(p1.y, p1.x) / deltaBin;
// 			float f = sobelMat.at<uchar>(p2.y, p2.x);
// 			v2 = sobelMat.at<uchar>(p2.y, p2.x) / deltaBin;
// 			v1 = v1 >= m_glcm.rows ? m_glcm.rows - 1 : v1;
// 			v2 = v2 >= m_glcm.cols ? m_glcm.cols - 1 : v2;
// 			if (v1 < 0)
// 				v1 = 0;
// 			if (v2 < 0)
// 				v2 = 0;
// 			m_glcm_sobel.at<float>(v1, v2)++;
// 		}
// 	}
	//////////////////////////////////////////////////////////////////////////
	///计算灰度共生矩阵的特征值
	/// 归一化灰度共生矩阵
	//////////////////////////////////////////////////////////////////////////
	cv::Mat feature(1, 6, CV_32FC1);
	normalize(m_glcm);
	normalize(m_glcm_sobel);
	feature.at<float>(0, 0) = energe(m_glcm);
	feature.at<float>(0, 1) = contrast(m_glcm);
	feature.at<float>(0, 2) = homogeneity(m_glcm);
	feature.at<float>(0, 3) = energe(m_glcm_sobel);
	feature.at<float>(0, 4) = contrast(m_glcm_sobel);
	feature.at<float>(0, 5) = homogeneity(m_glcm_sobel);
	return feature;
}

cv::Mat FlowerFeatureExtractor::extractColor()
{
	cv::Mat matRet;
	int hbins = 30, sbins = 35;
	int histSize[] = { hbins, sbins };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	cv::MatND hist;
	int channels[] = { 0, 1 };
	cv::calcHist(&m_hsvMat, 1, channels, m_binaryMat,
		hist, 2, histSize, ranges,
		true, // the histogram is uniform
		false );

	//归一化
	cv::normalize(hist, hist);

	double maxVal=0;
	cv::minMaxLoc(hist, 0, &maxVal, 0, 0);

	std::vector<std::pair<cv::Point2i, float> > fVec;

	for(int h = 0; h < hbins; h++)
	{
		float fSum = 0;
		for( int s = 0; s < sbins; s++ )
			fSum += hist.at<float>(h, s);
		matRet.push_back(fSum);
	}
	for (int s = 0; s < sbins; s++)
	{
		float fSum = 0;
		for( int h = 0; h < hbins; h++ )
			fSum += hist.at<float>(h, s);
		matRet.push_back(fSum);
	}
	matRet = matRet.reshape(1, 1);
	matRet.convertTo(matRet, CV_32F);
	return matRet;
}

cv::Mat FlowerFeatureExtractor::extractShapeDistance()
{
	/// 求中心
	cv::Point2d centroid(0, 0);
	vector<vector<cv::Point> > countours;

	cv::findContours(m_binaryMat.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
	double maxarea = 0;
	cv::Rect maxrect(0, 0, 2, 2);
	size_t max_index = -1;
	for (size_t i = 0; i < counts ; ++i)
	{
		cv::Rect rect = cv::boundingRect(countours.at(i));
		if(rect.area() > maxarea)
		{
			maxarea = rect.area();
			maxrect = rect;
			max_index = i;
		}
	}
	m_rect = maxrect;
	/// 求重心 坐标求和后求平均值
	int iCount = 0;
	int iX = 0;
	int iY = 0;
	int iBottom = maxrect.y + maxrect.height;
	int iRight	= maxrect.x + maxrect.width;
	int iLeft	= maxrect.x;
	int iTop	= maxrect.y;
	for(int r = maxrect.y; r <= iBottom; ++r)
	{
		for(int c = maxrect.x; c <= iRight; ++c)
		{
			if (m_binaryMat.at<uchar>(r, c) == 255)
			{
				iX += c;
				iY += r;
				iCount++;
			}
		}
	}
	centroid.x = iX * 1.0 / iCount;
	centroid.y = iY * 1.0 / iCount;
	m_centroid = centroid;

	vector<cv::Point> &max_countours = countours.at(max_index);
	std::vector<int> vecCounts;
	std::vector<float> vecDis;
	float fMax = 0;
	float fMin = 1000000;
	for (size_t t = 0; t < max_countours.size(); t++)
	{
		cv::Point p = max_countours.at(t);
		float fDis = sqrtf((p.x - m_centroid.x) * (p.x - m_centroid.x) + (p.y - m_centroid.y) * (p.y - m_centroid.y));
		vecDis.push_back(fDis);
		fMax = qMax(fDis, fMax);
		fMin = qMin(fDis, fMin);
	}
	cv::MatND hist;
	float hranges[] = { fMin, fMax };
	const float *ranges[] = { hranges };
	int channels[] = { 0 };
	int hbins = 15;
	int histSize[] = { hbins };
	cv::Mat matCounts(vecDis);
	matCounts.convertTo(matCounts, CV_32FC1);
	cv::calcHist(&matCounts, 1, channels, cv::Mat(),
		hist, 1, &hbins, ranges,
		true,
		false);
	
	hist = hist.reshape(1,1);
	return hist;
}

#define SIFTFEATURE_COUNT 100

cv::Mat FlowerFeatureExtractor::extractSift()
{
	cv::SIFT ext(SIFTFEATURE_COUNT);
	std::vector<cv::KeyPoint> keyPoints;
	cv::Mat descriptor;
	ext(m_grayMat, m_binaryMat, keyPoints, descriptor);
	return descriptor;
}
