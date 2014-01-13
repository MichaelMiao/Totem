#include "stdafx.h"
#include "FlowerFeatureExtractor.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <qglobal.h>
#include <QDebug>
#include "opencv2/highgui/highgui.hpp"
#define PI	3.1415926

using namespace cv;
cv::Mat FlowerFeatureExtractor::extractFeature(cv::Mat mat)
{
	m_srcMat = mat;
	m_glcm = cv::Mat::zeros(8, 8, CV_32F);
	m_glcm_sobel = cv::Mat::zeros(8, 8, CV_32F);

	cv::cvtColor(mat, m_grayMat, COLOR_BGR2GRAY);
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
	
	/// 花瓣个数
	vector<vector<cv::Point> > countoursExternal;
	cv::findContours(matRingA.clone(), countoursExternal, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	/// 花瓣之间的角度
	double fAngle = 0;
	std::vector<double> fVecAngle;
	for (size_t t = 0; t < countoursExternal.size(); t++)
	{
		double fTemp = 0;
		cv::Rect rc = cv::boundingRect(countoursExternal.at(t));
		int iX = rc.x + rc.width / 2;
		int iY = rc.y + rc.height / 2;
		if (iX == centroid.x)
		{
			fTemp = iY < centroid.y ? PI / 2 : (-PI / 2);
		}
		else
		{
			fTemp = atan((centroid.y - iY) / (iX - centroid.x));
			if (iX < centroid.x)
				fTemp += PI;
			else if (iX > centroid.x && iY > centroid.y)
				fTemp += PI * 2;
		}
//		cv::line(m_srcMat, cv::Point(iX, iY), centroid, cv::Scalar(0, 0, 0));
		fVecAngle.push_back(fTemp);
	}
//	cv::imwrite("I:/line.bmp", m_srcMat);
	for (size_t t = 0; t < fVecAngle.size(); t++)
		fAngle += qAbs(fVecAngle[t] - fVecAngle[(t + 1) % fVecAngle.size()]);
	
	fAngle /= countoursExternal.size();
	cv::Mat hsvimage;
	cv::cvtColor(m_srcMat, hsvimage, COLOR_BGR2HSV);
	cv::imwrite("I:/miao.bmp", hsvimage);
	std::vector<cv::Mat> vecHSV;
	cv::split(hsvimage, vecHSV);
	int d[100][100];
	memset(d, 0, sizeof(int) * 100 * 100);
	for (int r = iTop; r < iBottom; r++)
	{
		for (int c = iLeft; c < iRight; c++)
		{
			uchar dh	= vecHSV.at(0).at<uchar>(r, c);
			uchar ds	= vecHSV.at(1).at<uchar>(r, c);
			if (matRingB.at<uchar>(r, c) == 255 && (dh != 0 || ds != 0))
			{
				int hIndex = dh / 18.0;
				int sIndex = ds / 25.5;
				d[hIndex][sIndex]++;
			}
		}
	}
	cv::Mat fColor(1, 5, CV_32FC1);
	int hmax = 0, smax = 0, hmax2 = 0, smax2 = 0;
	float max_value = 0, max2_value = 0;
	for(int i = 0; i < 100; ++i)
	{
		for(int j = 0; j < 100; ++j)
		{
			if (max_value < d[i][j])
			{
				max_value = d[i][j];
				hmax = i;
				smax = j;
			}
			else if (d[i][j] > max2_value && d[i][j] < max_value)
			{
				max2_value = d[i][j];
				hmax2 = i;
				smax2 = j;
			}
		}
	}
	fColor.at<float>(0, 0) = hmax;
	fColor.at<float>(0, 1) = smax;
	fColor.at<float>(0, 2) = hmax2;
	fColor.at<float>(0, 3) = smax2;
	fColor.at<float>(0, 4) = max_value/(hsvimage.rows * hsvimage.cols + 1);
	/// 形状描述：尖锐度
	std::vector<cv::Point> vec = countours.at(max_index);
	float fSharpness = 0, sum = 0;
	for (size_t t = 0; t < vec.size(); t++)
	{
		sum += sqrt((vec.at(t).x - centroid.x) * (vec.at(t).x - centroid.x) +
						(vec.at(t).y - centroid.y) * (vec.at(t).y - centroid.y));
	}

	if(qFuzzyCompare(sum, 0))//等于0要单独处理一下
		sum = 1;
	fSharpness = vec.size() / sum;

	/// 求灰度共生矩阵(包括原灰度图的glcm和经过sobel算子滤波后的图像的glcm)
	cv::Mat sobelMat;
//	cv::Sobel(m_grayMat, sobelMat, -1, 1, 0);
	cv::Laplacian(m_grayMat, sobelMat, CV_16S, 5);
	cv::convertScaleAbs(sobelMat, sobelMat);

	double deltaBin = 255 / 8; /// 每个bin的范围
	cv::Point2i p1, p2;
	for (int sita = 0; sita < 360; ++sita)
	{
		for(int r = B_min; r < B_max; ++r)
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
			int v1 = m_grayMat.at<uchar>(p1.y, p1.x) / deltaBin;
			int v2 = m_grayMat.at<uchar>(p2.y, p2.x) / deltaBin;
			v1 = v1 >= m_glcm.rows ? m_glcm.rows - 1 : v1;
			v2 = v2 >= m_glcm.cols ? m_glcm.cols - 1 : v2;
			
			if (v1 < 0)
				v1 = 0;

			if (v2 < 0)
				v2 = 0;
			m_glcm.at<float>(v1, v2)++;

			v1 = sobelMat.at<uchar>(p1.y, p1.x) / deltaBin;
			float f = sobelMat.at<uchar>(p2.y, p2.x);
			v2 = sobelMat.at<uchar>(p2.y, p2.x) / deltaBin;
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
	//////////////////////////////////////////////////////////////////////////
	///计算灰度共生矩阵的特征值
	/// 归一化灰度共生矩阵
	//////////////////////////////////////////////////////////////////////////
	cv::Mat feature(1, 21, CV_32FC1);
	normalize(m_glcm);
	normalize(m_glcm_sobel);
	cv::Moments m = cv::moments(m_binaryMat, true);
	double dhu[7];
	cv::HuMoments(m, dhu);

	feature.at<float>(0, 0) = fColor.at<float>(0, 0);
	feature.at<float>(0, 1) = fColor.at<float>(0, 1);
	feature.at<float>(0, 2) = fColor.at<float>(0, 2);
	feature.at<float>(0, 3) = fColor.at<float>(0, 3);
	feature.at<float>(0, 4) = fColor.at<float>(0, 4);
	feature.at<float>(0, 5) = fSharpness;
	feature.at<float>(0, 6) = energe(m_glcm);
	feature.at<float>(0, 7) = contrast(m_glcm);
	feature.at<float>(0, 8) = homogeneity(m_glcm);
	feature.at<float>(0, 9) = energe(m_glcm_sobel);
	feature.at<float>(0, 10) = contrast(m_glcm_sobel);
	feature.at<float>(0, 11) = homogeneity(m_glcm_sobel);
	feature.at<float>(0, 12) = countoursExternal.size();
	feature.at<float>(0, 13) = fAngle;
	for (int i = 0; i < 7; i++)
	{
		feature.at<float>(0, 14 + i) = dhu[i];
	}
	return feature;
}

float FlowerFeatureExtractor::energe(const cv::Mat &glcm)
{
	float sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			sum += glcm.at<float>(i,j) * glcm.at<float>(i,j);
		}
	}
	return sum;
}

float FlowerFeatureExtractor::contrast(const cv::Mat &glcm)
{
	double sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			sum += glcm.at<float>(i,j) * (i - j) * (i - j);
		}
	}
	return sum;
}

float FlowerFeatureExtractor::homogeneity(const cv::Mat &glcm)
{
	float sum = 0;
	for (int i = 0; i < glcm.rows; ++i)
	{
		for(int j = 0; j < glcm.cols; ++j)
		{
			sum += glcm.at<float>(i,j) / (qAbs(i - j)  + 1);
		}
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
