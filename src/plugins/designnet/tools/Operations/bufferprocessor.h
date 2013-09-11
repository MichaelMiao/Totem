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
	BufferProcessor(DesignNet::DesignNetSpace *space, QObject* parent = 0);
	~BufferProcessor();
	virtual DesignNet::Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
signals:
protected:

	virtual void propertyChanged(DesignNet::Property *prop);
	BufferProcessorPrivate *d;
};

#endif // BUFFERPROCESSOR_H
