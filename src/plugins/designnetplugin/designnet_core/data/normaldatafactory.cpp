#include "normaldatafactory.h"
#include "designnetconstants.h"
#include "idata.h"
#include "imagedatawidget.h"

namespace DesignNet{

NormalDataFactory::NormalDataFactory(QObject *parent)
    : IDataWidgetFactory(parent)
{
}

IDataWidget *NormalDataFactory::createWidget(IData *property, QGraphicsItem* parent)
{
    if(property->id() == DesignNet::Constants::DATA_TYPE_IMAGE)
    {
        ImageDataWidget *widget = new ImageDataWidget(property, parent);
        return widget;
    }
    return 0;
}

Core::Id NormalDataFactory::id() const
{
    return DesignNet::Constants::DATA_WIDGET_FACTORY_NORMAL;
}

QList<Core::Id> NormalDataFactory::supportedTypes()
{
    QList<Core::Id> types;
    types << DesignNet::Constants::DATA_TYPE_IMAGE;
    return types;
}

}
