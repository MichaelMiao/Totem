#ifndef FEATUREEXTRACTION_H
#define FEATUREEXTRACTION_H

#include "featureextraction_global.h"
#include "designnet/designnet_core/designnetbase/processor.h"

#include <opencv2/core/core.hpp>
namespace DesignNet{
	class MatrixData;
	class DesignNetSpace;
}

namespace FeatureExtraction{
class GLCMBlockPrivate;
class FEATUREEXTRACTION_EXPORT GLCMBlock: public DesignNet::Processor
{
public:

	enum GLCMType{
		Normal,
		Polor
	};

	GLCMBlock(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	virtual ~GLCMBlock();
	DECLEAR_PROCESSOR(GLCMBlock)
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);			//!< 处理函数

protected:
	
	virtual void propertyChanged(DesignNet::Property *prop);

private:
	GLCMBlockPrivate *d;
};

}

#endif // FEATUREEXTRACTION_H
