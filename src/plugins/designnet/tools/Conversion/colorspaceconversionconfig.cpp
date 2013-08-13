#include "colorspaceconversionconfig.h"
#include "designnet/designnet_core/widgets/processorconfigpage.h"
using namespace DesignNet;
ColorSpaceConversionConfig::ColorSpaceConversionConfig(DesignNet::Processor* processor, QWidget *parent)
	: ProcessorConfigWidget(processor, parent)
{

}

ColorSpaceConversionConfig::~ColorSpaceConversionConfig()
{

}

ProcessorConfigWidget* ColorSpaceConversionConfig::create( DesignNet::Processor * processor, QWidget *parent /*= 0*/ )
{
	ColorSpaceConversionConfig *widget = new ColorSpaceConversionConfig(processor, parent);
	widget->addPropertyPage(processor);
	return widget;
}
