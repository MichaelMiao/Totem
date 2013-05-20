#include "normalpropertywidgetfactory.h"
#include "designnetconstants.h"
#include "pathdialogpropertywidget.h"
#include "ipropertywidget.h"
#include "optionpropertywidget.h"
#include "optionproperty.h"
#include "doublerangeproperty.h"
#include "doublerangepropertywidget.h"
#include "boolproperty.h"
#include "boolpropertywidget.h"
namespace DesignNet{

NormalPropertyWidgetFactory::NormalPropertyWidgetFactory(QObject *parent) :
    IPropertyWidgetFactory(parent)
{
}

IPropertyWidget *NormalPropertyWidgetFactory::createWidget(Property *property,
                                                         QWidget *parent)
{
	if(property->typeID() == DesignNet::Constants::PROPERTY_TYPE_PATHDIALOG)
	{
		PathDialogProperty *pathDialog = qobject_cast<PathDialogProperty*>(property);
		if(pathDialog)
			return new PathDialogPropertyWidget(pathDialog, parent);
	}
	else if(property->typeID() == DesignNet::Constants::PROPERTY_TYPE_OPTION)
	{
		OptionProperty *optionProperty = qobject_cast<OptionProperty*>(property);
		if(optionProperty)
			return new OptionPropertyWidget(optionProperty, parent);
	}
	else if(property->typeID() == DesignNet::Constants::PROPERTY_TYPE_DOUBLERANGE)
	{
		DoubleRangeProperty *doubleRangeProperty = qobject_cast<DoubleRangeProperty*>(property);
		return new DoubleRangePropertyWidget(doubleRangeProperty, parent);
	}
	else if(property->typeID() == DesignNet::Constants::PROPERTY_TYPE_BOOL)
	{
		BoolProperty *boolProperty = qobject_cast<BoolProperty*>(property);
		return new BoolPropertyWidget(boolProperty, parent);
	}
	return 0;
}

Core::Id NormalPropertyWidgetFactory::id() const
{
    return this->metaObject()->className();
}

QList<Core::Id> NormalPropertyWidgetFactory::supportedTypes()
{
    QList<Core::Id> supportedTypes;
    supportedTypes << DesignNet::Constants::PROPERTY_TYPE_PATHDIALOG;
    return supportedTypes;
}
}
