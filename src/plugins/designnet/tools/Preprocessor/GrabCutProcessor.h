// Author: Michael_BJFU
#pragma once
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "opencv2/core/core.hpp"
#include <QSemaphore>

class GrabCutFrontWidget;
class GrabCutProcessor : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(GrabCutProcessor)
	GrabCutProcessor(DesignNet::DesignNetSpace *space, QObject* parent = 0);
	virtual ~GrabCutProcessor();
	virtual QString title() const;
	virtual QString category() const;	//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);				//!< 处理函数
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

	void setOutputValue(cv::Mat mat);

protected:

	DesignNet::ProcessorFrontWidget* processorFrontWidget();

	virtual void propertyChanged(DesignNet::Property *prop);

private:
	
	GrabCutFrontWidget* m_pWidget;
	QSemaphore			m_semaphore;
};