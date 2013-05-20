#include "color2gray.h"
#include "designnetplugin/designnet_core/graphicsitem/portgraphicsitem.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace DesignNet;
namespace Conversion{

Color2Gray::Color2Gray(DesignNet::DesignNetSpace *space, QGraphicsItem *parent)
	: ProcessorGraphicsBlock(space, parent),
	m_outputPort(new ImageData(ImageData::IMAGE_GRAY, this), Port::OUT_PORT),
	m_inputPort(new ImageData(ImageData::IMAGE_BGR, this),Port::IN_PORT)
{
	m_inputPort.setName("InputColorImage");
	m_outputPort.setName("grayImage");
	addPort(&m_outputPort);
	addPort(&m_inputPort);
	setName(tr("Color 2 Gray"));
}

Color2Gray::~Color2Gray()
{

}

QString Color2Gray::title() const
{
	return tr("Color 2 Gray");
}

QString Color2Gray::category() const
{
	return tr("Conversion");
}

bool Color2Gray::process()
{
	emit logout("Color2Gray::process()");
	cv::Mat mat;
	cv::cvtColor(m_colorData.imageData(), mat, CV_RGB2GRAY);
	m_grayData.setImageData(mat);
	pushData(&m_grayData, "grayImage");
	return true;
}

void Color2Gray::dataArrived( Port* port )
{
	PortGraphicsItem *item = getPortGraphicsItem(port);
	if(item && port == &m_inputPort)
	{
		ImageData *idata = qobject_cast<ImageData *>(m_inputPort.data());
		if(idata)
		{
			m_colorData.setImageData(idata->imageData().clone());
			setDataReady(true);
		}
	}
}

void Color2Gray::propertyChanged( Property *prop )
{

}

Processor* Color2Gray::create( DesignNetSpace *space /*= 0*/ ) const
{
	return new Color2Gray(space);
}

bool Color2Gray::connectionTest( DesignNet::Port* src, DesignNet::Port* target )
{
	if(target == &m_inputPort)
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