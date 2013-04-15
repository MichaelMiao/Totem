#ifndef COLOR2GRAY_H
#define COLOR2GRAY_H

#include "designnetplugin/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnetplugin/designnet_core/designnetbase/port.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace Conversion{

class Color2Gray : public DesignNet::ProcessorGraphicsBlock
{
	Q_OBJECT

public:
	Color2Gray(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
	~Color2Gray();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<bool> &fi);     //!< 处理函数
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
	virtual void propertyChanged(DesignNet::Property *prop);
private:
	DesignNet::Port m_outputPort;	//!< 输出端口
	DesignNet::Port m_inputPort;	//!< 输入端口
	DesignNet::ImageData m_colorData;				//!< 彩色图
	DesignNet::ImageData m_grayData;				//!< 灰度图
};

}

#endif // COLOR2GRAY_H
