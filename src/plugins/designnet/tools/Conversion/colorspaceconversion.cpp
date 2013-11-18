#include "colorspaceconversion.h"
#include "designnet/designnet_core/property/optionproperty.h"
#include "designnet/designnet_core/designnetbase/designnetspace.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace DesignNet;

#define DATA_LABEL_INPUTIMAGE "3 Channel Image"
#define DATA_LABEL_OUTPUTIMAGE "3 Channel Image"

namespace Conversion{
class ColorSpaceConversionPrivate
{
public:
	ColorSpaceConversionPrivate(ColorSpaceConversion * parent);
	OptionProperty*			m_property;
	ColorSpaceConversion *	m_conversion;
};
ColorSpaceConversionPrivate::ColorSpaceConversionPrivate( ColorSpaceConversion * parent )
	: m_conversion(parent)
{
	m_property = new OptionProperty("ConversionOption", QLatin1String("ConversionOption"), parent);
}

ColorSpaceConversion::ColorSpaceConversion( DesignNetSpace *space, QObject *parent /*= 0*/ )
	: Processor(space, parent),
	d(new ColorSpaceConversionPrivate(this))
{
	//!< Ìí¼ÓÊôÐÔ
	d->m_property->addOption(tr("BGR 2 YCrCb"), cv::COLOR_BGR2YCrCb);
	d->m_property->addOption(tr("BGR 2 HSV"), cv::COLOR_BGR2HSV);
	d->m_property->addOption(tr("BGR 2 Lab"), cv::COLOR_BGR2Lab);
	d->m_property->addOption(tr("BGR 2 Luv"), cv::COLOR_BGR2Luv);

	d->m_property->addOption(tr("YCrCb 2 BGR"), cv::COLOR_YCrCb2BGR);
	d->m_property->addOption(tr("HSV 2 BGR"), cv::COLOR_HSV2BGR);
	d->m_property->addOption(tr("Lab 2 BGR"), cv::COLOR_Lab2BGR);
	d->m_property->addOption(tr("Luv 2 BGR"), cv::COLOR_Luv2BGR);
	d->m_property->setName(tr("Conversion"));
	d->m_property->select(tr("BGR 2 HSV"));
	addProperty(d->m_property);
	setName(tr("Color Space Conversion"));
}

ColorSpaceConversion::~ColorSpaceConversion()
{

}

Processor* ColorSpaceConversion::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new ColorSpaceConversion(space);
}

QString ColorSpaceConversion::title() const
{
	return tr("ColorSpaceConversion");
}

QString ColorSpaceConversion::category() const
{
	return tr("Conversion");
}

bool ColorSpaceConversion::process(QFutureInterface<ProcessResult> &future)
{
	int iType = d->m_property->value().toInt(0);
	if(iType != 0)
	{
		ImageData* imageData = qobject_cast<ImageData*>(getData("InputImage").at(0));
		cv::Mat mat;
		cv::cvtColor(imageData->imageData(), mat, iType);
		imageData->setImageData(mat);
		pushData(imageData, "OutputImage");
	}
	return true;
}

void ColorSpaceConversion::propertyChanged( Property *prop )
{

}

bool ColorSpaceConversion::connectionTest( DesignNet::Port* src, DesignNet::Port* target )
{
	if(target == &d->m_inputPort)
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

QMultiMap<QString, DesignNet::ProcessData> ColorSpaceConversion::datasNeeded()
{
	return ;
}

QMap<QString, DesignNet::ProcessData> ColorSpaceConversion::dataProvided()
{
	return ;
}

}
