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
	PortIndex_In,
	PortIndex_Out,
};

static PortData s_ports[] =
{
	{ Port::IN_PORT,	DATATYPE_MATRIX,	"Input Matrix" },
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
	ProcessData pd = getOneData(s_ports[PortIndex_In].strName);
	cv::Mat mat = pd.variant.value<cv::Mat>();
	cv::merge(m_mats, mat);
	pushData(qVariantFromValue(mat), DATATYPE_MATRIX, s_ports[PortIndex_Out].strName);
	return true;
}

void BufferProcessor::propertyChanged( DesignNet::Property *prop )
{

}
