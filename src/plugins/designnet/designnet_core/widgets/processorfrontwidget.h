#pragma once
#include <QWidget>
#include "../designnet_core_global.h"

namespace DesignNet
{
class Processor;
class DESIGNNET_CORE_EXPORT ProcessorFrontWidget : public QWidget
{
	Q_OBJECT

public:

	ProcessorFrontWidget(Processor* processor);
	~ProcessorFrontWidget();

private:

	Processor* m_processor;
};

}
