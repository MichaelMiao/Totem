#include "bufferprocessor.h"
#include "data/matrixdata.h"
#include "property/doublerangeproperty.h"
#include "data/intdata.h"
#include "opencv2/core/core.hpp"
#include <vector>
using namespace std;
using namespace DesignNet;

enum PortIndex
{
	PortIndex_In_Data,
	PortIndex_In_DataIndex,
	PortIndex_Out,
};

static PortData s_ports[] =
{
	{ Port::IN_PORT,	DATATYPE_MATRIX,	"Input Data" },
	{ Port::IN_PORT,	DATATYPE_INT,		"Data Index" },
	{ Port::OUT_PORT,	DATATYPE_MATRIX,	"Output Buffer" },
};

BufferProcessor::BufferProcessor( DesignNet::DesignNetSpace *space, QObject* parent /*= 0*/ )
	: DesignNet::Processor(space, parent)
{
	for (int i = 0; i < _countof(s_ports); i++)
		addPort(s_ports[i].ePortType, s_ports[i].eDataType, s_ports[i].strName);
	setName(tr("Buffer"));
}

BufferProcessor::~BufferProcessor()
{
}

QString BufferProcessor::title() const
{
	return tr("Buffer");
}

QString BufferProcessor::category() const
{
	return tr("Operations");
}

bool BufferProcessor::process(QFutureInterface<ProcessResult> &future)
{
	ProcessData pd = getOneData(s_ports[PortIndex_In_Data].strName);
	cv::Mat mat = pd.variant.value<cv::Mat>();
	mat = mat.reshape(1, 1);
	m_mats.push_back(mat);
	int iIndex = getPortData<int>(s_ports[PortIndex_In_DataIndex]);
	if (m_mats.rows != iIndex)
	{
		qDebug() << "Error";
	}
	if (iIndex == -1)
	{
		pushData(qVariantFromValue(m_mats), DATATYPE_MATRIX, s_ports[PortIndex_Out].strName);
		notifyProcess();
		return true;
	}
	emit childProcessFinished();
	return true;
}

void BufferProcessor::propertyChanged( DesignNet::Property *prop )
{

}

bool BufferProcessor::prepareProcess()
{
	m_mats = cv::Mat();
	return true;
}
