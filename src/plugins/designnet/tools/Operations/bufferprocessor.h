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
	virtual QString category() const;//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ������

signals:
protected:

	virtual void propertyChanged(DesignNet::Property *prop);
	std::vector<cv::Mat>		m_mats;
};

#endif // BUFFERPROCESSOR_H
