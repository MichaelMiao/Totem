// Author: miaojinquan
#pragma once
#include "../../../../designnet/designnet_core/designnetbase/port.h"
#include "../../../../designnet/designnet_core/designnetbase/processor.h"
#include "../../../../designnet/designnet_core/property/boolproperty.h"
#include "../../../designnet_core/property/doublerangeproperty.h"
#include "algrithom/opencvlibsvm.h"


using namespace DesignNet;
class SVMFrontWidget;
class SVMClassifer : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(SVMClassifer)

	explicit SVMClassifer(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~SVMClassifer();
	QString title() const;
	QString category() const;													   //!< 种类
	bool prepareProcess();
	bool process(QFutureInterface<DesignNet::ProcessResult> &future) override;     //!< 处理函数

	ProcessorFrontWidget* processorFrontWidget() override;

private:

	BoolProperty*			m_propBoolTrain;
	DoubleRangeProperty*	m_propDoubleRange;
	cv::Mat			m_matNormalize;
	OpenCVLibSVM	m_svm;
	SVMFrontWidget*			m_pWidget;
};
