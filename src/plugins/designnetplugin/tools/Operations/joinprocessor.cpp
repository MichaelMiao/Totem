#include "joinprocessor.h"
#include "graphicsitem/processorgraphicsblock.h"
#include "data/matrixdata.h"
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

JoinProcessor::JoinProcessor(DesignNet::DesignNetSpace *space, QGraphicsItem *parent)
	: DesignNet::ProcessorGraphicsBlock(space, parent),
	d(new JoinProcessorPrivate(this))
{
	setIcon(QIcon(QLatin1String(":/images/join.png")));
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

bool JoinProcessor::process()
{
	int iRows = 0, iCols = 0;
	foreach(Port *p, m_inputPorts)
	{
		if (p->data()->isValid())
		{
			cv::Mat &mat = ((MatrixData*)p->data())->getMatrix();
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

void JoinProcessor::dataArrived( DesignNet::Port* port )
{
	bool bComplete = true;
	foreach(Port *p, m_inputPorts)
	{
		if (!p->data()->isValid())
		{
			bComplete = false;
		}
	}
	if(bComplete)
	{
		setDataReady();
	}
}

void JoinProcessor::createContextMenu( QMenu *parentMenu )
{

}

void JoinProcessor::propertyChanged( Property *prop )
{
	
}
