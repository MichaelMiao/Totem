#include "stdafx.h"
#include "inputloader.h"
#include "ImageFolderLoader.h"
#include <QtPlugin>
#include "../../designnet_core\designnetbase\processorconfigmanager.h"
#include "imagefolderconfigwidget.h"
using namespace DesignNet;

namespace InputLoader{

bool InputLoaderPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	return true;
}

void InputLoaderPlugin::extensionsInitialized()
{
	m_imageFolderLoader = new ImageFolderLoader(0);

	ProcessorConfigManager::instance()->registerConfigWidget(m_imageFolderLoader->typeID().toString(),
		new ImageFolderConfigWidget(0, 0));

	addAutoReleasedObject(m_imageFolderLoader);
}

}
Q_EXPORT_PLUGIN2(InputLoader::InputLoaderPlugin, InputLoader::InputLoaderPlugin)
