// Author: Michael_BJFU
#pragma once
#include "../../../../designnet/designnet_core/designnetbase/port.h"
#include "../../../../designnet/designnet_core/designnetbase/processor.h"


using namespace DesignNet;
class ClassiferStatistic : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(ClassiferStatistic)

	explicit ClassiferStatistic(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~ClassiferStatistic();
	QString title() const;
	QString category() const;													   //!< ����
	bool prepareProcess();
	bool process(QFutureInterface<DesignNet::ProcessResult> &future) override;     //!< ������

};
