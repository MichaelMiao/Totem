#include "processorconfiglistener.h"
#include "property/property.h"
namespace DesignNet{
ProcessorConfigListener::ProcessorConfigListener(QObject *parent)
	: QObject(parent)
{

}

ProcessorConfigListener::~ProcessorConfigListener()
{

}

void ProcessorConfigListener::onSettingsChanged()
{

}

void ProcessorConfigListener::onPropertyChanged( Property *prop )
{

}

}
