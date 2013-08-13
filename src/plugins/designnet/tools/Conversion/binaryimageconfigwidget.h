#ifndef BINARYIMAGECONFIGWIDGET_H
#define BINARYIMAGECONFIGWIDGET_H

#include <QObject>
#include "designnet/designnet_core/widgets/processorconfigwidget.h"
class BinaryImageConfigWidget : public DesignNet::ProcessorConfigWidget
{
	Q_OBJECT

public:
	BinaryImageConfigWidget(DesignNet::Processor* processor, QWidget *parent);
	~BinaryImageConfigWidget();
	virtual ProcessorConfigWidget* create(DesignNet::Processor * processor, QWidget *parent = 0);
private:
	
};

#endif // BINARYIMAGECONFIGWIDGET_H
