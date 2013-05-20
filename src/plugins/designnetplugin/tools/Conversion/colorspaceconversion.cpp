#include "colorspaceconversion.h"
#include "designnetplugin/designnet_core/property/optionproperty.h"
#include "designnetplugin/designnet_core/designnetbase/designnetspace.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace DesignNet;
namespace Conversion{
class ColorSpaceConversionPrivate
{
public:
	ColorSpaceConversionPrivate(ColorSpaceConversion * parent);
	OptionProperty*			m_property;
	Port					m_inputPort;
	Port					m_outputPort;
	ImageData*				m_imageData;	//!< 
	ColorSpaceConversion *	m_conversion;
};

ColorSpaceConversionPrivate::ColorSpaceConversionPrivate( ColorSpaceConversion * parent )
	: m_conversion(parent),
	m_inputPort(new ImageData(ImageData::IMAGE_BGR, parent), Port::IN_PORT),
	m_outputPort(new ImageData(ImageData::IMAGE_BGR, parent), Port::OUT_PORT)
{
	m_property = new OptionProperty("ConversionOption", QLatin1String("ConversionOption"), parent);
	m_inputPort.setName(QLatin1String("InputImage"));
	m_outputPort.setName(QLatin1String("OutputImage"));
	m_imageData = new ImageData(ImageData::IMAGE_BGR, parent);
}

ColorSpaceConversion::ColorSpaceConversion( DesignNetSpace *space, QGraphicsItem *parent /*= 0*/ )
	: ProcessorGraphicsBlock(space, parent),
	d(new ColorSpaceConversionPrivate(this))
{
	//!< 添加属性
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
	//!< 添加端口
	addPort(&d->m_inputPort);
	addPort(&d->m_outputPort);
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

bool ColorSpaceConversion::process()
{
	int iType = d->m_property->value().toInt(0);
	if(iType != 0)
	{
		cv::Mat mat;
		cv::cvtColor(d->m_imageData->imageData(), mat, iType);
		d->m_imageData->setImageData(mat);
		pushData(d->m_imageData, "OutputImage");
	}
	return true;
}

void ColorSpaceConversion::dataArrived( DesignNet::Port* port )
{
	if(port == &d->m_inputPort)
	{
		ImageData *data = qobject_cast<ImageData *>(port->data());
		if(data)
		{
			d->m_imageData->setImageData(data->imageData().clone());
			setDataReady(true);
		}
	}
	ProcessorGraphicsBlock::dataArrived(port);
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

}
