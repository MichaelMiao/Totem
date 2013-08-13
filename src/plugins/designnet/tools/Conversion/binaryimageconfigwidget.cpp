#include "binaryimageconfigwidget.h"
#include "designnet/designnet_core/widgets/processorconfigpage.h"
using namespace DesignNet;
BinaryImageConfigWidget::BinaryImageConfigWidget(Processor* processor, QWidget *parent)
	: ProcessorConfigWidget(processor, parent)
{

}

BinaryImageConfigWidget::~BinaryImageConfigWidget()
{

}

ProcessorConfigWidget* BinaryImageConfigWidget::create( DesignNet::Processor * processor, 
	QWidget *parent /*= 0*/ )
{
	BinaryImageConfigWidget *widget = new BinaryImageConfigWidget(processor, parent);
	widget->addPropertyPage(processor);
	return widget;
}
