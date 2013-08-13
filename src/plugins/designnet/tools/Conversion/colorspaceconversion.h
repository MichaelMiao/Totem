#ifndef COLORSPACECONVERSION_H
#define COLORSPACECONVERSION_H

#include "designnet/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"
#include <opencv2/core/core.hpp>

namespace Conversion{
class ColorSpaceConversionPrivate;
class ColorSpaceConversion : public DesignNet::Processor
{
	Q_OBJECT

public:
	ColorSpaceConversion(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	virtual ~ColorSpaceConversion();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process();     //!< 处理函数
	bool connectionTest( DesignNet::Port* src, DesignNet::Port* target );

public slots:
	virtual void propertyChanged(DesignNet::Property *prop);
private:
	ColorSpaceConversionPrivate *d;
};
}

#endif // COLORSPACECONVERSION_H
