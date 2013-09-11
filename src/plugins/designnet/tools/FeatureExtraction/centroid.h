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
class CentroidPrivate;
class Centroid :  public DesignNet::Processor
{
	Q_OBJECT

public:
	Centroid(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	virtual ~Centroid();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);			//!< ������

protected:
	virtual void propertyChanged(DesignNet::Property *prop);
private:
	CentroidPrivate *d;
};
}
#endif // CENTROID_H
