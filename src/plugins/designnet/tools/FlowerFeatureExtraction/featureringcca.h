#ifndef FEATURERINGACC_H
#define FEATURERINGACC_H

#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{
	class MatrixData;
}

namespace FlowerFeatureExtraction{
// ����ͨ����ͨ�ɷָ���
class FeatureRingCCA : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(FeatureRingCCA)
	FeatureRingCCA(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~FeatureRingCCA();
	QString title() const;
	QString category() const;//!< ����
	bool process(QFutureInterface<DesignNet::ProcessResult> &future) override;     //!< ������

protected:

	void propertyChanged(DesignNet::Property *prop) override;
	bool connectionTest(DesignNet::Port* src, DesignNet::Port* target) override;
};

}

#endif // FEATURERINGACC_H
