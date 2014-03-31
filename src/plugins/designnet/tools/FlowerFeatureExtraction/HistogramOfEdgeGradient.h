// Author: miaojinquan
#pragma once
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"


namespace FlowerFeatureExtraction{

class HistogramOfEdgeGradient : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(HistogramOfEdgeGradient)

	HistogramOfEdgeGradient(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~HistogramOfEdgeGradient();
	QString title() const;
	QString category() const;//!< 种类
	bool process(QFutureInterface<DesignNet::ProcessResult> &future) override;     //!< 处理函数
};

}
