#include "featureringcca.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/matrixdata.h"
#include "designnet/designnet_core/designnetbase/port.h"
using namespace DesignNet;
namespace FlowerFeatureExtraction{

FeatureRingCCA::FeatureRingCCA(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent)
{

}

FeatureRingCCA::~FeatureRingCCA()
{

}

Processor* FeatureRingCCA::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new FeatureRingCCA(space);
}

QString FeatureRingCCA::title() const
{
	return tr("Connected component count Of Ring A");
}

QString FeatureRingCCA::category() const
{
	return tr("Flowers/Feature");
}

bool FeatureRingCCA::process()
{
	return true;
}

void FeatureRingCCA::propertyChanged( DesignNet::Property *prop )
{

}

bool FeatureRingCCA::connectionTest( DesignNet::Port* src, DesignNet::Port* target )
{
	return true;
}

}

