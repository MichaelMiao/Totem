#include "color2gray.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace DesignNet;
namespace Conversion{
#define DATA_LABEL_COLORIMAGE	"ColorImage"
#define DATA_LABEL_GRAYIMAGE	"GrayImage"


Color2Gray::Color2Gray(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent)
{
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
// 	ImageData *idata = qobject_cast<ImageData *>(getData("InputColorImage").at(0));
// 	if(idata)
// 	{
// 		cv::Mat mat;
// 		cv::cvtColor(idata->imageData(), mat, CV_RGB2GRAY);
// 		ImageData grayData;
// 		grayData.setImageData(mat);
// 		pushData(&grayData, "grayImage");
// 		emit logout("Color2Gray::process() OK");
// 		return true;
// 	}
 	return false;
}

void Color2Gray::propertyChanged( Property *prop )
{

}

Processor* Color2Gray::create( DesignNetSpace *space /*= 0*/ ) const
{
	return new Color2Gray(space);
}

QMultiMap<QString, DesignNet::ProcessData> Color2Gray::datasNeeded()
{
	QMultiMap<QString, ProcessData> ret;
	ProcessData pd;
	pd.strLabel = DATA_LABEL_COLORIMAGE;
	pd.dataType = DATATYPE_8UC3IMAGE;
	ret.insert(DATA_LABEL_COLORIMAGE, pd);
	return ret;
}

QMap<QString, DesignNet::ProcessData> Color2Gray::dataProvided()
{
	QMap<QString, DesignNet::ProcessData> ret;
	ProcessData pd;
	pd.strLabel = DATA_LABEL_GRAYIMAGE;
	pd.dataType = DATATYPE_MATRIX;
	ret[DATA_LABEL_GRAYIMAGE] = pd;
	return ret;
}

}