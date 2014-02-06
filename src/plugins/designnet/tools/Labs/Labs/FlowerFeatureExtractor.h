#pragma once
#include "opencv2/core/core.hpp"


/*
	f[0]			外连通环连通成分个数
	f[1] - f[5]		颜色特征
	f[6]			形状特征
	f[7] - f[12]	纹理特征提取
*/

class FlowerFeatureExtractor
{
public:

	cv::Mat extractFeature(cv::Mat mat);
	cv::Mat siftFeature(cv::Mat mat);
	void normalize(cv::Mat &glcm);

	float energe(const cv::Mat &glcm);//!< 计算灰度共生矩阵的能量
	float contrast(const cv::Mat &glcm);//!<  计算对比度
	float homogeneity(const cv::Mat &glcm);//!< 计算同质性，GLCM中元素对角线分布的控件紧密度的度量

	cv::Mat extractShape(cv::Mat matBinary);
	cv::Mat extractShape2(cv::Mat matBinary);
	cv::Mat getGradient();
	cv::Mat extractGLCM();
	cv::Mat extractColor();

private:

	cv::Mat		m_srcMat;
	cv::Mat		m_grayMat;
	cv::Mat		m_binaryMat;
	cv::Mat		m_hsvMat;
	cv::Mat		m_glcm;					//!< 灰度共生矩阵
	cv::Mat		m_glcm_sobel;			//!< sobel滤波后的灰度共生矩阵
	cv::Rect	m_rect;
	cv::Point	m_centroid;
};