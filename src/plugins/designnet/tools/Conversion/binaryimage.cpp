#include "binaryimage.h"
#include "designnet/designnet_core/graphicsitem/portgraphicsitem.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/matrixdata.h"
#include "designnet/designnet_core/property/doublerangeproperty.h"

#include "ConversionConstants.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <QIcon>
using namespace DesignNet;
namespace Conversion{

BinaryImage::BinaryImage( DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/ )
	: DesignNet::Processor(space, parent),
	m_inputPort(new ImageData(ImageData::IMAGE_GRAY, this), Port::IN_PORT, "Gray Image"),
	m_outputPort(new ImageData(ImageData::IMAGE_BINARY, this), Port::OUT_PORT, "Binary Image")
{
	addPort(&m_inputPort);
	addPort(&m_outputPort);
	m_doubleRangeProperty = new DesignNet::DoubleRangeProperty("threshold", "threshold",this);
	addProperty(m_doubleRangeProperty);
	setName(tr("Binary Image"));
	setIcon(QLatin1String(":/medias/binary.png"));
}

BinaryImage::~BinaryImage()
{

}

Processor* BinaryImage::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new BinaryImage(space);
}

QString BinaryImage::title() const
{
	return tr("BinaryImage");
}

QString BinaryImage::category() const
{
	return tr("Conversion");
}

bool BinaryImage::process()
{
	ImageData *srcData = qobject_cast<ImageData*>(m_inputPort.data());
	cv::Mat mat = srcData->imageData();
	double valueThreshold = m_doubleRangeProperty->value();///阈值
	cv::Mat binaryImage;
	cv::Mat grayMat = srcData->imageData();
	
	cv::threshold(grayMat, binaryImage, valueThreshold, 255, CV_THRESH_BINARY);
	MatrixData data;
	data.setMatrix(binaryImage);
	data.setIndex(srcData->index());
	pushData(&data, "Binary Image");
	return true;
}

void BinaryImage::propertyChanged( DesignNet::Property *prop )
{

}

/**
 * \fn	bool BinaryImage::connectionTest( DesignNet::Port* src, DesignNet::Port* target )
 *
 * \brief	连接测试，只能接受灰度图.
 *
 * \author	Michael_BJFU
 * \date	2013/5/29
 *
 * \param [in,out]	src   	源端口.
 * \param [in,out]	target	目标端口.
 *
 * \return	如果源端口数据是灰度图返回true，否则false.
 */

bool BinaryImage::connectionTest( DesignNet::Port* src, DesignNet::Port* target )
{
	if(target == &m_inputPort)
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
