#ifndef HSCOLORHISTOGRAM_H
#define HSCOLORHISTOGRAM_H

#include "designnetplugin/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnetplugin/designnet_core/designnetbase/port.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{
class MatrixData;
}
namespace FlowerFeatureExtraction{
class HSColorFeature : public DesignNet::ProcessorGraphicsBlock
{
	Q_OBJECT

public:
	HSColorFeature(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
	~HSColorFeature();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process();     //!< 处理函数
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
	virtual void propertyChanged(DesignNet::Property *prop);
	virtual bool connectionTest(DesignNet::Port* src, DesignNet::Port* target);
private:
	DesignNet::Port m_outputPort;	//!< 输出端口
	DesignNet::Port m_inputPort;	//!< 输入端口
	DesignNet::ImageData	m_imageData;	//!< HSV彩色空间数据
	cv::Mat					m_hist;			//!< 直方图
	DesignNet::MatrixData *	m_featureData;	//!< 特征数据
	
};

}

#endif // HSCOLORHISTOGRAM_H
