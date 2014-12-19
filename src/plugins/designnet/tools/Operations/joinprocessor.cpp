#include "joinprocessor.h"
#include <QIcon>
#include "data/matrixdata.h"
#include "Utils/XML/xmlserializer.h"
#include "../../designnet_core/designnetbase/port.h"


using namespace DesignNet;
enum PortIndex
{
	PortIndex_Out,
};

static PortData s_ports[] =
{
	{ Port::OUT_PORT,	DATATYPE_MATRIX,	"Joined Matrix" },
};

using namespace DesignNet;
JoinProcessor::JoinProcessor(DesignNet::DesignNetSpace *space, QObject* parent)
	: DesignNet::Processor(space, parent)
{
	setIcon(QLatin1String(":/images/join.png"));
	setName(tr("Join"));
	for (int i = 0; i < _countof(s_ports); i++)
		addPort(s_ports[i].ePortType, s_ports[i].eDataType, s_ports[i].strName);
	m_bResizableInput = true;
}

JoinProcessor::~JoinProcessor()
{
}

QString JoinProcessor::title() const
{
	return tr("Join");
}

QString JoinProcessor::category() const
{
	return tr("Operations");
}

bool JoinProcessor::process(QFutureInterface<ProcessResult> &future)
{
	int iRows = 0, iCols = 0;
	QList<Port*> ports = getPorts(Port::IN_PORT);
	QList<Port*>::iterator itr = ports.begin();
	cv::Mat matOut;
	while (itr != ports.end())
	{
		cv::Mat m = (*itr)->getInputData()->variant.value<cv::Mat>();
		if (m.empty())
		{
			emit logout(tr("Port %1 data error!").arg((*itr)->name()));
			return false;
		}

		m = m.reshape(1, m.cols * m.rows);
		m.convertTo(m, CV_32FC1);
		if (matOut.rows == 0)
			matOut.push_back(m);
		else
			matOut.push_back(m);
		itr++;
	}
	matOut = matOut.reshape(1, 1);
	cv::FileStorage fsfesture("I:/data/test2.xml", cv::FileStorage::WRITE);
	fsfesture << "minmax" << matOut;
	fsfesture.release();

	pushData(qVariantFromValue(matOut), DATATYPE_MATRIX, s_ports[PortIndex_Out].strName);
	notifyProcess();
	return true;
}

void JoinProcessor::propertyChanged( Property *prop )
{
	
}

void JoinProcessor::serialize( Utils::XmlSerializer& s ) const
{
	Processor::serialize(s);
	s.serialize("InputPortCount", m_inputPort.size());
}

void JoinProcessor::deserialize( Utils::XmlDeserializer& s )
{
	int iCount = 0;
	s.deserialize("InputPortCount", iCount);
	for (int i = 0; i < iCount; i++)
		addPort(Port::IN_PORT, DATATYPE_MATRIX, tr("Port(%1)").arg(i), true);
}
