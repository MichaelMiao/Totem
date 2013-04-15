#include "flowerfeatureextractionplugin.h"
#include <QtPlugin>
namespace FeatureExtraction{
FlowerFeatureExtractionPlugin::FlowerFeatureExtractionPlugin(QObject *parent)
{

}

FlowerFeatureExtractionPlugin::~FlowerFeatureExtractionPlugin()
{

}

void FlowerFeatureExtractionPlugin::extensionsInitialized()
{

}

bool FlowerFeatureExtractionPlugin::initialize( const QStringList &arguments, QString *errorString )
{
	return true;
}

}

Q_EXPORT_PLUGIN2(FeatureExtraction::FlowerFeatureExtractionPlugin, FeatureExtraction::FlowerFeatureExtractionPlugin)