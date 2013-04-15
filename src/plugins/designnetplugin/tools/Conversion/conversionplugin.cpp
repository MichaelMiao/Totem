#include "conversionplugin.h"
#include "color2gray.h"
#include "colorspaceconversion.h"
#include <QtPlugin>
namespace Conversion{

ConversionPlugin::ConversionPlugin()
{

}

ConversionPlugin::~ConversionPlugin()
{

}

bool ConversionPlugin::initialize( const QStringList &arguments, QString *errorString )
{
	return true;
}

void ConversionPlugin::extensionsInitialized()
{
	m_color2Gray = new Color2Gray(0);
	m_colorSpaceConversion = new ColorSpaceConversion(0);
	addAutoReleasedObject(m_color2Gray);
	addAutoReleasedObject(m_colorSpaceConversion);
}

}
Q_EXPORT_PLUGIN2(Conversion::ConversionPlugin, Conversion::ConversionPlugin)