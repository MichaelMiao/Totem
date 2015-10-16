#ifndef NORMALPROPERTYWIDGETFACTORY_H
#define NORMALPROPERTYWIDGETFACTORY_H
#include "ipropertywidgetfactory.h"


namespace DesignNet
{
class IPropertyWidget;
class NormalPropertyWidgetFactory : public IPropertyWidgetFactory
{
	Q_OBJECT

public:

	explicit NormalPropertyWidgetFactory(QObject* parent = 0);
	IPropertyWidget* createWidget(Property* property, QWidget* parent);
	QList<DesignNet::PropertyType> supportedTypes();

};
}

#endif // NORMALPROPERTYWIDGETFACTORY_H
