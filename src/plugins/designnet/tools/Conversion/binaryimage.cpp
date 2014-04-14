#include "binaryimage.h"
#include "../../designnet_core/designnetconstants.h"
#include "../../designnet_core/data/imagedata.h"
#include "../../designnet_core/data/matrixdata.h"
#include "../../designnet_core/data/datatype.h"
#include "../../designnet_core/property/doublerangeproperty.h"

#include "ConversionConstants.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <QIcon>
#include <QMultiMap>
#include "Utils/varianthelper.h"
using namespace DesignNet;
#define DATA_LABEL_GRAYIMAGE	_T("GrayImage")
#define DATA_LABEL_BINARYIMAGE	_T("BinaryImage")


namespace Conversion{


BinaryImage::BinaryImage( DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/ )
	: DesignNet::Processor(space, parent)
{
	m_doubleRangeProperty = new DesignNet::DoubleRangeProperty("threshold", "threshold",this);
	addProperty(m_doubleRangeProperty);
	addPort(Port::IN_PORT, DATATYPE_GRAYIMAGE, DATA_LABEL_GRAYIMAGE);
	addPort(Port::OUT_PORT, DATATYPE_BINARYIMAGE, DATA_LABEL_BINARYIMAGE);
	setName(tr("Binary Image"));
	setIcon(QLatin1String(":/medias/binary.png"));
}

BinaryImage::~BinaryImage()
{

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
	notifyDataWillChange();
	cv::Mat grayMat = getOneData(DATA_LABEL_GRAYIMAGE).variant.value<cv::Mat>();
	double valueThreshold = m_doubleRangeProperty->value();///ãÐÖµ
	cv::Mat binaryImage;
	
	cv::threshold(grayMat, binaryImage, valueThreshold, 255, CV_THRESH_BINARY);
	pushData(qVariantFromValue(binaryImage), DATATYPE_BINARYIMAGE, DATA_LABEL_BINARYIMAGE);
	notifyProcess();
	return true;
}

void BinaryImage::propertyChanged( DesignNet::Property *prop )
{

}
}
