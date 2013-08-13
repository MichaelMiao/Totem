#ifndef CONVERSION_H
#define CONVERSION_H

#include "conversion_global.h"
#include "extensionsystem/iplugin.h"

namespace Conversion{
class Color2Gray;
class ColorSpaceConversion;
class BinaryImage;
class CONVERSION_EXPORT ConversionPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
public:
	ConversionPlugin();
	~ConversionPlugin();
	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();
private:
	Color2Gray*				m_color2Gray;
	ColorSpaceConversion*	m_colorSpaceConversion;
	BinaryImage*			m_binaryImage;
};
}

#endif // CONVERSION_H
