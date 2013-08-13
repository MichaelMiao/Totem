#ifndef CONFIGLISTENER_H
#define CONFIGLISTENER_H

#include <QObject>
#include "designnet_core_global.h"
namespace DesignNet{
class Property;
class DESIGNNET_CORE_EXPORT ProcessorConfigListener : public QObject
{
	Q_OBJECT
	friend class ProcessorConfigWidget;
	friend class ProcessorConfigPage;
public:
	ProcessorConfigListener(QObject *parent);
	~ProcessorConfigListener();
protected:
	virtual void onSettingsChanged();
	virtual void onPropertyChanged(Property *prop);
private:
	
};
}


#endif // CONFIGLISTENER_H
