#ifndef HSCOLORHISTOGRAM_H
#define HSCOLORHISTOGRAM_H

#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{
class MatrixData;
}
namespace FlowerFeatureExtraction{
class HSColorFeature : public DesignNet::Processor
{
	Q_OBJECT

public:
	HSColorFeature(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~HSColorFeature();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ������
protected:
	virtual void propertyChanged(DesignNet::Property *prop);
	virtual bool connectionTest(DesignNet::Port* src, DesignNet::Port* target);
private:
	DesignNet::Port m_outputPort;	//!< ����˿�
	DesignNet::Port m_inputPort;	//!< ����˿�
	DesignNet::ImageData	m_imageData;	//!< HSV��ɫ�ռ�����
	cv::Mat					m_hist;			//!< ֱ��ͼ
	DesignNet::MatrixData *	m_featureData;	//!< ��������
	
};

}

#endif // HSCOLORHISTOGRAM_H
