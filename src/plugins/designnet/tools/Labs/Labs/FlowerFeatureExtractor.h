#pragma once
#include "opencv2/core/core.hpp"


/*
	f[0]			����ͨ����ͨ�ɷָ���
	f[1] - f[5]		��ɫ����
	f[6]			��״����
	f[7] - f[12]	����������ȡ
*/

class FlowerFeatureExtractor
{
public:

	cv::Mat extractFeature(cv::Mat mat);
	cv::Mat siftFeature(cv::Mat mat);
	void normalize(cv::Mat &glcm);

	float energe(const cv::Mat &glcm);//!< ����Ҷȹ������������
	float contrast(const cv::Mat &glcm);//!<  ����Աȶ�
	float homogeneity(const cv::Mat &glcm);//!< ����ͬ���ԣ�GLCM��Ԫ�ضԽ��߷ֲ��Ŀؼ����ܶȵĶ���

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
	cv::Mat		m_glcm;					//!< �Ҷȹ�������
	cv::Mat		m_glcm_sobel;			//!< sobel�˲���ĻҶȹ�������
	cv::Rect	m_rect;
	cv::Point	m_centroid;
};