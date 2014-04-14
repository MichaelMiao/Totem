#ifndef BUFFERPROCESSOR_H
#define BUFFERPROCESSOR_H

#include <QObject>
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
class BufferProcessorPrivate;
class BufferProcessor : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(BufferProcessor)

	BufferProcessor(DesignNet::DesignNetSpace *space, QObject* parent = 0);
	~BufferProcessor();
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool prepareProcess();		//!< 处理之前的准备,这里会确保数据已经准备好了
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数

signals:
protected:

	virtual void propertyChanged(DesignNet::Property *prop);
	cv::Mat		m_mats;
};

#endif // BUFFERPROCESSOR_H
