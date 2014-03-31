#ifndef CENTROID_H
#define CENTROID_H

#include "featureextraction_global.h"
#include "designnet/designnet_core/designnetbase/processor.h"

#include <opencv2/core/core.hpp>

namespace DesignNet{
	class MatrixData;
	class DesignNetSpace;
}
namespace FeatureExtraction{
class Centroid :  public DesignNet::Processor
{
	Q_OBJECT

public:
	
	DECLEAR_PROCESSOR(Centroid)

	Centroid(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	virtual ~Centroid() {}
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);			//!< 处理函数

protected:
	virtual void propertyChanged(DesignNet::Property *prop);
private:
};
}
#endif // CENTROID_H
