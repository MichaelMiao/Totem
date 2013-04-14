#include "totem_gui_pch.h"
#include "inputloaderplugin.h"
#include "graphicsnormalimageloader.h"
#include "ImageFolderLoader.h"
#include <QtPlugin>
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
    addAutoReleasedObject(m_normalImageLoader);
	addAutoReleasedObject(m_imageFolderLoader);
}
}
Q_EXPORT_PLUGIN2(InputLoader::InputLoaderPlugin, InputLoader::InputLoaderPlugin)
