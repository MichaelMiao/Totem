#include "bufferprocessor.h"
#include "data/matrixdata.h"
#include "property/doublerangeproperty.h"
#include "data/intdata.h"
#include "opencv2/core/core.hpp"
#include <vector>
using namespace std;
using namespace DesignNet;
class BufferProcessorPrivate
{
public: 
	BufferProcessorPrivate(BufferProcessor *processor)
		: m_processor(processor),
		m_bufferSizePort(new IntData(10, processor), Port::IN_PORT, "Buffer Size"),
		m_inputBufferPort(new MatrixData(processor), Port::IN_PORT, "Input Buffer"),
		m_outputMatrixPort(new MatrixData(processor), Port::OUT_PORT, "Output Matrix")
	{
		m_inputBufferPort.setMultiInputSupported(true);
	}
	DesignNet::Port				m_bufferSizePort;
	DesignNet::Port				m_inputBufferPort;
	DesignNet::Port				m_outputMatrixPort;
	BufferProcessor*			m_processor;
	std::vector<cv::Mat>		m_mats;
};


BufferProcessor::BufferProcessor( DesignNet::DesignNetSpace *space, QObject* parent /*= 0*/ )
	: DesignNet::Processor(space, parent),
	d(new BufferProcessorPrivate(this))
{
	addPort(&d->m_bufferSizePort);
	addPort(&d->m_inputBufferPort);
	addPort(&d->m_outputMatrixPort);
	setName(tr("Buffer"));
}

BufferProcessor::~BufferProcessor()
{
	delete d;
}

DesignNet::Processor* BufferProcessor::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new BufferProcessor(space);
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
	QVector<IData*> datas = getData("Input Buffer");
	if (datas.size() == 0)
	{
		emit logout("There's no input data.");
		return false;
	}
	foreach (IData* data, datas)
	{
		MatrixData *mat = (MatrixData*)data;
		d->m_mats.push_back(mat->getMatrix());
	}
	IntData* dataCount = (IntData*)d->m_bufferSizePort.getInputData().at(0);
	Q_ASSERT(dataCount->value() > 0);
	if (d->m_mats.size() < dataCount->value())
		return true;
	
	cv::Mat mat;
	cv::merge(d->m_mats, mat);
	MatrixData data;
	data.setMatrix(mat);
	pushData(&data, "Output Matrix");
	d->m_mats.clear();
	return true;
}

void BufferProcessor::propertyChanged( DesignNet::Property *prop )
{

}
