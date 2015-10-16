#include "stdafx.h"
#include "normalpropertywidgetfactory.h"
#include "coreplugin/Common/id.h"
#include "boolproperty.h"
#include "doublerangeproperty.h"
#include "optionproperty.h"
#include "property.h"
#include "../widgets/boolpropertywidget.h"
#include "../widgets/ipropertywidget.h"

namespace DesignNet
{
NormalPropertyWidgetFactory::NormalPropertyWidgetFactory(QObject* parent) :
	IPropertyWidgetFactory(parent)
{
}

IPropertyWidget* NormalPropertyWidgetFactory::createWidget(Property* property,
    QWidget* parent)
{
	if (property->propertyType() == DesignNet::Property_Bool)
	{
		BoolProperty* boolProperty = qobject_cast<BoolProperty*>(property);
		return new BoolPropertyWidget(boolProperty, parent);
	}

	return 0;
}

QList<DesignNet::PropertyType> NormalPropertyWidgetFactory::supportedTypes()
{
	QList<DesignNet::PropertyType> supportedTypes;
	supportedTypes << DesignNet::Property_Bool;
	return supportedTypes;
}
}
