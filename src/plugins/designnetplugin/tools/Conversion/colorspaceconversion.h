#ifndef COLORSPACECONVERSION_H
#define COLORSPACECONVERSION_H

#include "designnetplugin/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnetplugin/designnet_core/designnetbase/port.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>

namespace Conversion{
class ColorSpaceConversionPrivate;
class ColorSpaceConversion : public DesignNet::ProcessorGraphicsBlock
{
	Q_OBJECT

public:
	ColorSpaceConversion(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
	virtual ~ColorSpaceConversion();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<bool> &fi);     //!< 处理函数
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
public slots:
	virtual void propertyChanged(DesignNet::Property *prop);
private:
	ColorSpaceConversionPrivate *d;
};
}

#endif // COLORSPACECONVERSION_H
