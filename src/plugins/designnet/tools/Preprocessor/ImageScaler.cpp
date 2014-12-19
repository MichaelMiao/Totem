#include "ImageScaler.h"
#include "../../designnet_core/designnetbase/port.h"


#define CONST_HEIGHT	240
#define CONST_WIDTH		320
using namespace DesignNet;

enum PortIndex
{
	PortIndex_In_InputMat,
	PortIndex_In_OutputMat,
};

static PortData s_ports[] =
{
	{ Port::IN_PORT,	DATATYPE_8UC3IMAGE,		"Input Image" },
	{ Port::OUT_PORT,	DATATYPE_8UC3IMAGE,		"Output Image" },
};

static void scaleImage(cv::Mat &mat, cv::Size sz)
{
	double fScale	= qMax(sz.height * 1.0 / mat.rows, sz.width * 1.0 / mat.cols);
	cv::resize(mat, mat, cv::Size(mat.rows * fScale, mat.cols * fScale));
}

ImageScaler::ImageScaler(DesignNet::DesignNetSpace *space, QObject* parent /*= 0*/)
{
	for (int i = 0; i < _countof(s_ports); i++)
		addPort(s_ports[i].ePortType, s_ports[i].eDataType, s_ports[i].strName);
	setName(tr("ImageScaler"));
}

ImageScaler::~ImageScaler()
{

}

QString ImageScaler::title() const
{
	return tr("ImageScaler");
}

QString ImageScaler::category() const
{
	return tr("Preprocessor");
}

bool ImageScaler::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	ProcessData data = getOneData(s_ports[PortIndex_In_InputMat].strName);
	cv::Mat mat = data.variant.value<cv::Mat>();

	return true;
}

void ImageScaler::serialize(Utils::XmlSerializer& s) const
{
	__super::serialize(s);
}

void ImageScaler::deserialize(Utils::XmlDeserializer& s)
{
	__super::deserialize(s);
}
