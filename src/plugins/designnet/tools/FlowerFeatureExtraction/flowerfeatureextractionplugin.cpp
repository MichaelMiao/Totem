#include "flowerfeatureextractionplugin.h"
#include <QtPlugin>
#include "featureringcca.h"
#include "HistogramOfEdgeGradient.h"
#include "hscolorfeature.h"
#include "polorglcmblock.h"


namespace FlowerFeatureExtraction{
FlowerFeatureExtractionPlugin::FlowerFeatureExtractionPlugin(QObject *parent)
{

}

FlowerFeatureExtractionPlugin::~FlowerFeatureExtractionPlugin()
{

}

void FlowerFeatureExtractionPlugin::extensionsInitialized()
{
	m_featureProcessor	= new HSColorFeature(0);
	m_polorGLCM			= new PolorGLCMBlock(0);
	m_featureRingCCA	= new FeatureRingCCA(0);
	addAutoReleasedObject(m_featureProcessor);
	addAutoReleasedObject(m_polorGLCM);
	addAutoReleasedObject(m_featureRingCCA);
	addAutoReleasedObject(new HistogramOfEdgeGradient(0));
}

bool FlowerFeatureExtractionPlugin::initialize( const QStringList &arguments, QString *errorString )
{
	return true;
}

}

Q_EXPORT_PLUGIN2(FlowerFeatureExtraction::FlowerFeatureExtractionPlugin, FlowerFeatureExtraction::FlowerFeatureExtractionPlugin)
