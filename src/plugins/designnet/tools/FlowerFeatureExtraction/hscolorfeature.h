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

	DECLEAR_PROCESSOR(HSColorFeature)
	HSColorFeature(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~HSColorFeature();
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数

protected:
	
	virtual void propertyChanged(DesignNet::Property *prop);

	cv::Mat extractColor(cv::Mat& hsvMat, cv::Mat& binaryMat);

private:

	DesignNet::ImageData	m_imageData;	//!< HSV彩色空间数据
	cv::Mat					m_hist;			//!< 直方图
	DesignNet::MatrixData *	m_featureData;	//!< 特征数据
	
};

}

#endif // HSCOLORHISTOGRAM_H
