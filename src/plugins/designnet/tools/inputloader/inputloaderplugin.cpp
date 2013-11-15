#include "inputloaderplugin.h"
#include "graphicsnormalimageloader.h"
#include "ImageFolderLoader.h"
#include "imagefolderconfigwidget.h"
#include "designnetbase/processorconfigmanager.h"
#include "imageloaderconfigwidget.h"
#include <QtPlugin>
using namespace DesignNet;
namespace InputLoader{

InputLoaderPlugin::InputLoaderPlugin(QObject *parent)
{
}

InputLoaderPlugin::~InputLoaderPlugin()
{
}

bool InputLoaderPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    return true;
}

void InputLoaderPlugin::extensionsInitialized()
{
    m_normalImageLoader = new GraphicsNormalImageLoader(0);
	m_imageFolderLoader = new ImageFolderLoader(0);
	ProcessorConfigManager::instance()->registerConfigWidget(m_imageFolderLoader->typeID().toString(),
		new ImageFolderConfigWidget(0, 0));
	ProcessorConfigManager::instance()->registerConfigWidget(m_normalImageLoader->typeID().toString(),
		new ImageLoaderConfigWidget(0, 0));

    addAutoReleasedObject(m_normalImageLoader);
	addAutoReleasedObject(m_imageFolderLoader);
}
}
Q_EXPORT_PLUGIN2(InputLoader::InputLoaderPlugin, InputLoader::InputLoaderPlugin)
