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
	FeatureRingCCA(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~FeatureRingCCA();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ������
protected:
	virtual void propertyChanged(DesignNet::Property *prop);
	virtual bool connectionTest(DesignNet::Port* src, DesignNet::Port* target);

private:
	
	DesignNet::Port m_inputBinaryImagePort;	//!< �����ֵͼ�˿�
	DesignNet::Port m_inputCentroidPort;	//!< ��������λ�ö˿�

};

}

#endif // FEATURERINGACC_H
