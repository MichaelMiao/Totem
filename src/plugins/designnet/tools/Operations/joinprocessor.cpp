#include "joinprocessor.h"
#include "data/matrixdata.h"
#include "Utils/XML/xmlserializer.h"
#include <QIcon>
using namespace DesignNet;
class JoinProcessorPrivate
{
public:
	JoinProcessorPrivate(JoinProcessor *processor);
	~JoinProcessorPrivate();
	Port *m_outputPort;
};

JoinProcessorPrivate::JoinProcessorPrivate(JoinProcessor *processor)
{
	m_outputPort = new Port(new MatrixData(processor), Port::OUT_PORT, "OutputMatrix");
}

JoinProcessorPrivate::~JoinProcessorPrivate()
{
	delete m_outputPort;
}

JoinProcessor::JoinProcessor(DesignNet::DesignNetSpace *space, QObject* parent)
	: DesignNet::Processor(space, parent),
	d(new JoinProcessorPrivate(this))
{
	setIcon(QLatin1String(":/images/join.png"));
	setName(tr("Join"));
	setPortCountResizable(true);
	addPort(d->m_outputPort);
}

JoinProcessor::~JoinProcessor()
{
	delete d;
	d = 0;
}

DesignNet::Processor* JoinProcessor::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new JoinProcessor(space);
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
	foreach(Port *p, m_inputPorts)
	{
		if (p->getInputData().at(0)->isValid())
		{
			cv::Mat &mat = ((MatrixData*)p->getInputData().at(0))->getMatrix();
			mat = mat.reshape(1, 1);
			iCols += mat.cols;
		}
	}
	cv::Mat mat(1, iCols, CV_32FC1);
	iCols = 0;
	foreach(Port *p, m_inputPorts)
	{
		if (p->data()->isValid())
		{
			cv::Mat &t = ((MatrixData*)p->data())->getMatrix();
			t.copyTo(mat(cv::Rect(iCols, 0, t.cols, 1)));
			iCols += t.cols;
		}
	}
	MatrixData data;
	data.setMatrix(mat);
	pushData(&data, "OutputMatrix");
	return true;
}

void JoinProcessor::propertyChanged( Property *prop )
{
	
}

void JoinProcessor::serialize( Utils::XmlSerializer& s ) const
{
	s.serialize("count", m_inputPorts.size());
}

void JoinProcessor::deserialize( Utils::XmlDeserializer& s )
{
	int iCount = 0;
	s.deserialize("count", iCount);
	for (int i = 0; i < iCount; i++)
	{
		blockSignals(true);
		QString portName = "Port %1";
		Port *port = new Port(new MatrixData(0), Port::IN_PORT, portName.arg(i), this);
		addPort(port);
		blockSignals(false);
	}
}
