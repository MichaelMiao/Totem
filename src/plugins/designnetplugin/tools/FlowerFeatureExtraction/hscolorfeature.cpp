#include "hscolorfeature.h"
#include "designnetplugin/designnet_core/graphicsitem/portgraphicsitem.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include "designnetplugin/designnet_core/data/matrixdata.h"
#include "designnetplugin/designnet_core/designnetbase/port.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace DesignNet;
namespace FlowerFeatureExtraction{

HSColorFeature::HSColorFeature(DesignNet::DesignNetSpace *space, QGraphicsItem *parent)
	: ProcessorGraphicsBlock(space, parent),
	m_outputPort(new MatrixData(this), Port::OUT_PORT, tr("OutFeature")),
	m_inputPort(new ImageData(ImageData::IMAGE_BGR, this),Port::IN_PORT)
{
	m_featureData = new DesignNet::MatrixData(this);
	addPort(&m_inputPort);
	addPort(&m_outputPort);
	setName(tr("Color Feature HSV"));
}

HSColorFeature::~HSColorFeature()
{

}

Processor* HSColorFeature::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new HSColorFeature(space);
}

QString HSColorFeature::title() const
{
	return tr("Color Feature HSV");
}

QString HSColorFeature::category() const
{
	return tr("Flowers/Feature");
}

bool HSColorFeature::process()
{
	qDebug() << "HSColorFeature process";
	cv::Mat &hsvimage = m_imageData.imageData();
	int hbins = 30, sbins = 32;
	int histSize[] = {hbins, sbins};
	int channels[] = {0, 1};
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	cv::calcHist( &hsvimage, 1, channels, cv::Mat(), // do not use mask
		m_hist, 2, histSize, ranges,
		true, // the histogram is uniform
		false );
	cv::Mat feature(1, 5, CV_32FC1);
	int hmax = 0, smax = 0, hmax2 = 0, smax2 = 0;
	float max_value = 0, max2_value = 0;

	for(int i = 0; i < hbins; ++i)
	{
		for(int j = 0; j < sbins; ++j)
		{
			float f = m_hist.at<float>(i, j);
			
			if(f > max2_value )/// 大于次大值
			{
				if(f > max_value)/// 大于最大值
				{
					hmax = i;
					smax = j;
					max_value = f;
				}
				else
				{
					hmax2 = i;
					smax2 = j;
					max2_value = f;
				}
			}
		}
	}
	feature.at<float>(0) = hmax;
	feature.at<float>(1) = smax;
	feature.at<float>(2) = hmax2;
	feature.at<float>(3) = smax2;
	feature.at<float>(4) = max_value/(hsvimage.rows * hsvimage.cols + 1);
	MatrixData matrixData;
	matrixData.setMatrix(feature);
	pushData(&matrixData, "OutFeature");
	return true;
}

void HSColorFeature::dataArrived( DesignNet::Port* port )
{
	
	if(port == &m_inputPort)
	{
		m_imageData.copy(port->data());
		setDataReady(true);
	}
	ProcessorGraphicsBlock::dataArrived(port);
}

void HSColorFeature::propertyChanged( DesignNet::Property *prop )
{

}

bool HSColorFeature::connectionTest( Port* src, Port* target )
{
	if (target == &m_inputPort)
	{
		ImageData *srcData = qobject_cast<ImageData*>(src->data());
		if (!srcData || srcData->imageType() != ImageData::IMAGE_BGR)
		{
			return false;
		}
		return true;
	}
	return false;
}

}
