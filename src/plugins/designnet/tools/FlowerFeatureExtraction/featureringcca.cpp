#include "featureringcca.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/matrixdata.h"
#include "designnet/designnet_core/designnetbase/port.h"
using namespace DesignNet;
namespace FlowerFeatureExtraction{

FeatureRingCCA::FeatureRingCCA(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent)
{
	addPort(Port::IN_PORT, DATATYPE_BINARYIMAGE, "Binary Image");
	addPort(Port::IN_PORT, DATATYPE_MATRIX, "Centroid");
	setName(tr("Connected component count Of Ring A"));
}

FeatureRingCCA::~FeatureRingCCA()
{

}

QString FeatureRingCCA::title() const
{
	return tr("CCA");
}

QString FeatureRingCCA::category() const
{
	return tr("Flowers/Feature");
}

bool FeatureRingCCA::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	notifyDataWillChange();
	cv::Mat binaryImage = ((MatrixData*)getInputData(DATATYPE_MATRIX).at(0).variant.value<IData*>())->getMatrix();
	cv::Mat centroidMat = ((MatrixData*)getInputData(DATATYPE_BINARYIMAGE).at(0).variant.value<IData*>())->getMatrix();
	/// ÖÐÐÄ
	cv::Point2d centroid(0, 0);
	centroid.x = centroidMat.at<float>(0, 0);
	centroid.y = centroidMat.at<float>(0, 1);
	notifyProcess();

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

