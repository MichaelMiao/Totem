#ifndef FEATUREEXTRACTION_H
#define FEATUREEXTRACTION_H

#include "featureextraction_global.h"
#include "designnetplugin/designnet_core/graphicsitem/processorgraphicsblock.h"

#include <opencv2/core/core.hpp>
namespace DesignNet{
	class MatrixData;
	class DesignNetSpace;
}

namespace FeatureExtraction{
class GLCMBlockPrivate;
class FEATUREEXTRACTION_EXPORT GLCMBlock: public DesignNet::ProcessorGraphicsBlock
{
public:

	enum GLCMType{
		Normal,
		Polor
	};

	GLCMBlock(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
	virtual ~GLCMBlock();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process();			//!< 处理函数
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
	virtual void propertyChanged(DesignNet::Property *prop);
	bool connectionTest( DesignNet::Port* src, DesignNet::Port* target );
private:
	GLCMBlockPrivate *d;
};

}

#endif // FEATUREEXTRACTION_H
