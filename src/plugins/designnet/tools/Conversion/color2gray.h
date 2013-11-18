#ifndef COLOR2GRAY_H
#define COLOR2GRAY_H

#include "designnet/designnet_core/graphicsitem/processorgraphicsblock.h"
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
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
protected:
	virtual QMultiMap<QString, DesignNet::ProcessData> datasNeeded();
	virtual QMap<QString, DesignNet::ProcessData> dataProvided();
	virtual void propertyChanged(DesignNet::Property *prop);

};

}

#endif // COLOR2GRAY_H
