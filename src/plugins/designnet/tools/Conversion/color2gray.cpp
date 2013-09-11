#include "color2gray.h"
#include "designnet/designnet_core/graphicsitem/portgraphicsitem.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace DesignNet;
namespace Conversion{

Color2Gray::Color2Gray(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent),
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

bool Color2Gray::process(QFutureInterface<ProcessResult> &future)
{
	emit logout("Color2Gray::process()");
	ImageData *idata = qobject_cast<ImageData *>(getData("InputColorImage").at(0));
	if(idata)
	{
		cv::Mat mat;
		cv::cvtColor(idata->imageData(), mat, CV_RGB2GRAY);
		ImageData grayData;
		grayData.setImageData(mat);
		pushData(&grayData, "grayImage");
		emit logout("Color2Gray::process() OK");
		return true;
	}
	return false;
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