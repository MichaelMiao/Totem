#include "GrabCutProcessor.h"
#include "grabcutfrontwidget.h"
#include "../../designnet_core/designnetbase/port.h"

using namespace DesignNet;
enum PortIndex
{
//	PortIndex_In_InputMat,
	PortIndex_In_OutputMat,
};

static PortData s_ports[] =
{
//	{ Port::IN_PORT,	DATATYPE_8UC3IMAGE,		"Input Image" },
	{ Port::OUT_PORT,	DATATYPE_8UC3IMAGE,		"Output Image" },
};

using namespace DesignNet;
GrabCutProcessor::GrabCutProcessor(DesignNet::DesignNetSpace *space, QObject* parent)
	: Processor(space), m_pWidget(NULL)
{
	setName(tr("GrabCut"));
	m_pWidget = new GrabCutFrontWidget(this);
	for (int i = 0; i < _countof(s_ports); i++)
		addPort(s_ports[i].ePortType, s_ports[i].eDataType, s_ports[i].strName);
}

GrabCutProcessor::~GrabCutProcessor()
{

}

QString GrabCutProcessor::title() const
{
	return tr("GrabCut");
}

QString GrabCutProcessor::category() const
{
	return tr("Preprocessor");
}

bool GrabCutProcessor::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
//	m_semaphore.acquire();
	notifyProcess();
	return true;
}

void GrabCutProcessor::serialize(Utils::XmlSerializer& s) const
{
	Processor::serialize(s);
}

void GrabCutProcessor::deserialize(Utils::XmlDeserializer& s)
{
	Processor::deserialize(s);
}

void GrabCutProcessor::propertyChanged(DesignNet::Property *prop)
{

}

ProcessorFrontWidget* GrabCutProcessor::processorFrontWidget()
{
	return m_pWidget;
}

void GrabCutProcessor::setOutputValue(cv::Mat mat)
{
	pushData(qVariantFromValue(mat), DATATYPE_8UC3IMAGE, s_ports[PortIndex_In_OutputMat].strName);
}
