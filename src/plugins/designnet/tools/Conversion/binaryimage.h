#ifndef BINARYIMAGE_H
#define BINARYIMAGE_H

#include "designnet/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{
class DoubleRangeProperty;
}
namespace Conversion{
class BinaryImage : public DesignNet::Processor
{
	Q_OBJECT

public:
	BinaryImage(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~BinaryImage();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process();     //!< 处理函数
protected:
	virtual void propertyChanged(DesignNet::Property *prop);
	virtual bool connectionTest(DesignNet::Port* src, DesignNet::Port* target);
	
	DesignNet::Port m_outputPort;	//!< 输出端口
	DesignNet::Port m_inputPort;	//!< 输入端口
	DesignNet::ImageData m_binaryData;				//!< 二值图
	DesignNet::ImageData m_grayData;				//!< 灰度图
	DesignNet::DoubleRangeProperty* m_doubleRangeProperty; //!< 阈值
};

}

#endif // BINARYIMAGE_H
