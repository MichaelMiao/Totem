#include "stdafx.h"
#include "inputloader.h"
#include <QtPlugin>
#include "../../designnet_core/designnetbase/processorconfigmanager.h"
#include "graphicsnormalimageloader.h"
#include "ImageFolderLoader.h"
#include "LabelLoader.h"


using namespace DesignNet;

namespace InputLoader{

bool InputLoaderPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	return true;
}

void InputLoaderPlugin::extensionsInitialized()
{
	m_imageFolderLoader = new ImageFolderLoader(0);
	m_imageLoader		= new GraphicsNormalImageLoader(0);
	m_labelLoader		= new LabelLoader(0);
	addAutoReleasedObject(m_imageFolderLoader);
	addAutoReleasedObject(m_imageLoader);
	addAutoReleasedObject(m_labelLoader);
}

}
Q_EXPORT_PLUGIN2(InputLoader::InputLoaderPlugin, InputLoader::InputLoaderPlugin)
