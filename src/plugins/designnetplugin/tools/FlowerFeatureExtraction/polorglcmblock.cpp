#include "polorglcmblock.h"
#include "designnetplugin/designnet_core/data/matrixdata.h"
#include "designnetplugin/designnet_core/property/doublerangeproperty.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
using namespace std;
using namespace DesignNet;
namespace FlowerFeatureExtraction{

PolorGLCMBlock::PolorGLCMBlock(DesignNet::DesignNetSpace *space, QGraphicsItem *parent)
	: ProcessorGraphicsBlock(space, parent),
	m_outputPort(new MatrixData(this), Port::OUT_PORT, QLatin1String("OutputFeature")),
	m_inputPort(new ImageData(ImageData::IMAGE_GRAY, this), Port::IN_PORT, QLatin1String("InputGrayImage"))
{
	addPort(&m_outputPort);
	addPort(&m_inputPort);
	setName(tr("Polor GLCM"));
	m_doubleRangeProperty = new DoubleRangeProperty(QLatin1String("threshold"),
		tr("Threshold"), this);
	m_binCountProperty = new DoubleRangeProperty(QLatin1String("binCount"),
		tr("Bins"), this);
	m_binCountProperty->setRange(1, 255);
	m_binCountProperty->setDecimals(0);
	m_binCountProperty->setValue(8);
	addProperty(m_doubleRangeProperty);
	addProperty(m_binCountProperty);

	m_glcm = cv::Mat::zeros(m_binCountProperty->value(),m_binCountProperty->value(), CV_32F);
	m_glcm_sobel = cv::Mat::zeros(m_binCountProperty->value(),m_binCountProperty->value(), CV_32F);
}

PolorGLCMBlock::~PolorGLCMBlock()
{

}

Processor* PolorGLCMBlock::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new PolorGLCMBlock(space);
}

QString PolorGLCMBlock::title() const
{
	return tr("Polor GLCM");
}

QString PolorGLCMBlock::category() const
{
	return tr("Flowers/Feature");
}

bool PolorGLCMBlock::process()
{
	ImageData *grayImage = qobject_cast<ImageData*>(m_inputPort.data());
	if(!grayImage)
		return false;
	if(grayImage->imageData().type() != CV_8UC1)/// 只处理灰度图
	{
		emit logout(tr("The input image should be a gray level image"));
		return false;
	}
	/// 二值化图像，可以方便找出中心点
	double valueThreshold = m_doubleRangeProperty->value();///阈值
	cv::Mat binaryImage;
	cv::Mat grayMat = grayImage->imageData();
	cv::threshold(grayImage->imageData(), binaryImage, valueThreshold, 255, CV_THRESH_BINARY);
	cv::imwrite("C:/gray.jpg", binaryImage);
	/// 求中心
	cv::Point2d centroid(0, 0);
	vector<vector<cv::Point> > countours;
	cv::findContours(binaryImage.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
	if(counts <= 0)
		return false;/// 找到的轮廓数不能为0
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
	int iRight = maxrect.x + maxrect.width;
	for(int r = maxrect.y; r < iBottom; ++r)
	{
		for(int c = maxrect.x; c < iRight; ++c)
		{
			if (binaryImage.at<uchar>(r, c) == 255)
			{
				iX += c;
				iY += r;
				iCount++;
			}
		}
	}
	centroid.x = iX / iCount;
	centroid.y = iY / iCount;
	m_centroid = centroid;
	double radius = qMin(iRight - centroid.x, iBottom - centroid.y);/// 半径
	
	/// 求灰度共生矩阵(包括原灰度图的glcm和经过sobel算子滤波后的图像的glcm)
	cv::Mat sobelMat;
	cv::Sobel(grayMat, sobelMat, -1, 1, 0);

	double B_min = radius / 3;
	double B_max = radius * 2 / 3;
	double deltaBin = 255/ m_binCountProperty->value(); /// 每个bin的范围
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
				p2.x >= grayMat.cols || p1.x >= grayMat.cols ||
				p2.y >= grayMat.rows || p1.y >= grayMat.rows)
			{
				continue;
			}
			///找到P1, p2 两个点的灰度级
			int v1 = grayMat.at<uchar>(p1.y, p1.x)/deltaBin;
			int v2 = grayMat.at<uchar>(p2.y, p2.x)/deltaBin;
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
	//////////////////////////////////////////////////////////////////////////
	///计算灰度共生矩阵的特征值
	/// 归一化灰度共生矩阵
	//////////////////////////////////////////////////////////////////////////

	/// 求裴勇论文中的F[6] ,边缘轮廓点的个数/（边缘轮廓点到中心的距离之和）
	double sum = 0;
	vector<cv::Point>::iterator itr = countours.at(max_index).begin();
	vector<cv::Point>::iterator end = countours.at(max_index).end();
	while(itr != end)
	{
		sum += sqrt( ( (*itr).x - centroid.x) * ( (*itr).x - centroid.x) + ( (*itr).y - centroid.y) * ( (*itr).y - centroid.y));
		itr++;
	}
	if(qFuzzyCompare(sum, 0))//等于0要单独处理一下
		sum = 1;
	cv::Mat feature(1, 7, CV_32FC1);
	feature.at<float>(0, 0) = countours.at(max_index).size() / sum;///边缘轮廓点的个数/（边缘轮廓点到中心的距离之和）
	normalize(m_glcm);
	feature.at<float>(0, 1) = energe(m_glcm);
	feature.at<float>(0, 2) = contrast(m_glcm);
	feature.at<float>(0, 3) = homogeneity(m_glcm);
	normalize(m_glcm_sobel);
	feature.at<float>(0, 4) = energe(m_glcm_sobel);
	feature.at<float>(0, 5) = contrast(m_glcm_sobel);
	feature.at<float>(0, 6) = homogeneity(m_glcm_sobel);
	MatrixData data;
	data.setMatrix(feature);
	pushData(&data, QLatin1String("OutputFeature"));
	return true;
}

void PolorGLCMBlock::dataArrived( DesignNet::Port* port )
{
	if(port == &m_inputPort)
	{
		
		setDataReady(true);
	}
}

void PolorGLCMBlock::propertyChanged( DesignNet::Property *prop )
{
	if(prop == m_binCountProperty)
	{
		m_glcm = cv::Mat::zeros(m_binCountProperty->value(),m_binCountProperty->value(), CV_32FC1);
		m_glcm_sobel = cv::Mat::zeros(m_binCountProperty->value(),m_binCountProperty->value(), CV_32FC1);
	}
	
}

void PolorGLCMBlock::normalize(cv::Mat &glcm)
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
			glcm.at<float>(i,j) = glcm.at<float>(i,j) /sum;
		}
	}
}

float PolorGLCMBlock::energe(const cv::Mat &glcm)
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

void PolorGLCMBlock::centroid()
{

}

float PolorGLCMBlock::homogeneity( const cv::Mat &glcm )
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

float PolorGLCMBlock::contrast( const cv::Mat &glcm )
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

bool PolorGLCMBlock::connectionTest( Port* src, Port* target )
{
	if (target == &m_inputPort)
	{
		ImageData *srcData = qobject_cast<ImageData*>(src->data());
		if (!srcData || srcData->imageType() != ImageData::IMAGE_GRAY)
		{
			return false;
		}
		return true;
	}
	return false;
}



}

