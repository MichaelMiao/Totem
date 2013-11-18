#include "binaryimage.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/matrixdata.h"
#include "designnet/designnet_core/data/datatype.h"
#include "designnet/designnet_core/property/doublerangeproperty.h"

#include "ConversionConstants.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <QIcon>
#include <QMultiMap>
using namespace DesignNet;
#define DATA_LABEL_GRAYIMAGE "GrayImage"
#define DATA_LABEL_BINARYIMAGE "BinaryImage"


namespace Conversion{


BinaryImage::BinaryImage( DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/ )
	: DesignNet::Processor(space, parent)
{
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

bool BinaryImage::process(QFutureInterface<ProcessResult> &future)
{
	ImageData *srcData = (ImageData*)(getInputData(DATA_LABEL_GRAYIMAGE).at(0).variant.value<void*>());
	cv::Mat mat = srcData->imageData();
	double valueThreshold = m_doubleRangeProperty->value();///ãÐÖµ
	cv::Mat binaryImage;
	cv::Mat grayMat = srcData->imageData();
	
	cv::threshold(grayMat, binaryImage, valueThreshold, 255, CV_THRESH_BINARY);
	ImageData data;
	data.setImageData(binaryImage);
	data.setIndex(srcData->index());
	pushData(10, DATA_LABEL_BINARYIMAGE);
	return true;
}

void BinaryImage::propertyChanged( DesignNet::Property *prop )
{

}

QMap<QString, DesignNet::ProcessData> BinaryImage::dataProvided()
{
	QMap<QString, DesignNet::ProcessData> ret;
	ProcessData pd;
	pd.strLabel = DATA_LABEL_BINARYIMAGE;
	pd.dataType = DATATYPE_MATRIX;
	ret[DATA_LABEL_BINARYIMAGE] = pd;
	return ;
}

QMultiMap<QString, ProcessData> BinaryImage::datasNeeded()
{
	QMultiMap<QString, ProcessData> ret;
	ProcessData pd;
	pd.strLabel = DATA_LABEL_GRAYIMAGE;
	pd.dataType = DATATYPE_GRAYIMAGE;
	ret.insert(DATA_LABEL_GRAYIMAGE, pd);
	return ret;
}


}
