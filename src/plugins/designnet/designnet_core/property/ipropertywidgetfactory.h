#ifndef PROPERTYWIDGETFACTORY_H
#define PROPERTYWIDGETFACTORY_H

#include <QObject>
#include "designnet_core_def.h"
#include "widgets/ipropertywidget.h"


namespace DesignNet
{

class DESIGNNET_CORE_EXPORT IPropertyWidgetFactory : public QObject
{
	Q_OBJECT

public:

	explicit IPropertyWidgetFactory(QObject* parent = 0) : QObject(parent) {}
	virtual ~IPropertyWidgetFactory() {}
	virtual IPropertyWidget* createWidget(Property* property, QWidget* parent) = 0;
	virtual QList<DesignNet::PropertyType> supportedTypes() = 0;

};
}

#endif // PROPERTYWIDGETFACTORY_H
