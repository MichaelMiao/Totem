#ifndef COLORSPACECONVERSIONCONFIG_H
#define COLORSPACECONVERSIONCONFIG_H

#include <QObject>
#include "designnet/designnet_core/widgets/processorconfigwidget.h"
class ColorSpaceConversionConfig : public DesignNet::ProcessorConfigWidget
{
	Q_OBJECT

public:
	ColorSpaceConversionConfig(DesignNet::Processor* processor, QWidget *parent);
	~ColorSpaceConversionConfig();
	virtual ProcessorConfigWidget* create(DesignNet::Processor * processor, QWidget *parent = 0);
private:
	
};

#endif // COLORSPACECONVERSIONCONFIG_H
