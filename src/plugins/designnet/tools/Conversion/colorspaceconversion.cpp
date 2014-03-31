#include "colorspaceconversion.h"
#include "designnet/designnet_core/property/optionproperty.h"
#include "designnet/designnet_core/designnetbase/designnetspace.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "Utils/varianthelper.h"
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
	addPort(Port::IN_PORT, DATATYPE_8UC3IMAGE, tr("ColorImage"));
	addPort(Port::OUT_PORT, DATATYPE_8UC3IMAGE, tr("ColorImage"));
	setName(tr("Color Space Conversion"));
}

ColorSpaceConversion::~ColorSpaceConversion()
{

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
	notifyDataWillChange();
	int iType = d->m_property->value().toInt(0);
	if(iType != 0)
	{
		ImageData* imageData = qobject_cast<ImageData*>(getData(tr("ColorImage")).at(0)->variant.value<IData*>());
		cv::Mat mat;
		cv::cvtColor(imageData->imageData(), mat, iType);
		imageData->setImageData(mat);
		QVariant var = VariantPointCvt<ImageData>::fromPtr(imageData);
		pushData(var, DATATYPE_8UC3IMAGE, "ColorImage");
	}
	notifyProcess();
	return true;
}

void ColorSpaceConversion::propertyChanged( Property *prop )
{

}

}
