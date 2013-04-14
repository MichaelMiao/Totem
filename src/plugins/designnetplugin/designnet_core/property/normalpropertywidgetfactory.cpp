#include "normalpropertywidgetfactory.h"
#include "designnetconstants.h"
#include "pathdialogpropertywidget.h"
#include "ipropertywidget.h"
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
