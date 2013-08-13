#ifndef COLOR2GRAY_H
#define COLOR2GRAY_H

#include "designnet/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>
namespace Conversion{

class Color2Gray : public DesignNet::Processor
{
	Q_OBJECT

public:
	Color2Gray(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~Color2Gray();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process();     //!< 处理函数
protected:
	virtual void propertyChanged(DesignNet::Property *prop);
	virtual bool connectionTest(DesignNet::Port* src, DesignNet::Port* target);
private:
	DesignNet::Port m_outputPort;	//!< 输出端口
	DesignNet::Port m_inputPort;	//!< 输入端口
};

}

#endif // COLOR2GRAY_H
