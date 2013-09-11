#include "featureringcca.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/matrixdata.h"
#include "designnet/designnet_core/designnetbase/port.h"
using namespace DesignNet;
namespace FlowerFeatureExtraction{

FeatureRingCCA::FeatureRingCCA(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent),
	m_inputBinaryImagePort(0, Port::IN_PORT),
	m_inputCentroidPort(0, Port::IN_PORT)
{
	addPort(&m_inputBinaryImagePort);
	addPort(&m_inputCentroidPort);
	setName(tr("Connected component count Of Ring A"));
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
	return tr("CCA");
}

QString FeatureRingCCA::category() const
{
	return tr("Flowers/Feature");
}

bool FeatureRingCCA::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	QVector<IData*> datas = m_inputCentroidPort.getInputData();
	Q_ASSERT(datas.size() == 1);
	cv::Mat binaryImage = ((MatrixData*)m_inputBinaryImagePort.getInputData().at(0))->getMatrix();
	cv::Mat centroidMat = ((MatrixData*)m_inputCentroidPort.getInputData().at(0))->getMatrix();
	/// ÖÐÐÄ
	cv::Point2d centroid(0, 0);
	centroid.x = centroidMat.at<float>(0, 0);
	centroid.y = centroidMat.at<float>(0, 1);

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

