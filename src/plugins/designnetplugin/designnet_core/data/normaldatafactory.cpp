#include "normaldatafactory.h"
#include "designnetconstants.h"
#include "idata.h"
#include "imagedatawidget.h"
#include "matrixdatawidget.h"
namespace DesignNet{

NormalDataFactory::NormalDataFactory(QObject *parent)
    : IDataWidgetFactory(parent)
{
}

IDataWidget *NormalDataFactory::createWidget(IData *property, QGraphicsItem* parent)
{
	IDataWidget* widget = 0;
    if(property->id() == DesignNet::Constants::DATA_TYPE_IMAGE)
    {
        widget = new ImageDataWidget(property, parent);
    }
	else if(property->id() == DesignNet::Constants::DATA_TYPE_MATRIX)
	{
		widget = new MatrixDataWidget(property, parent);
	}

    return widget;
}

Core::Id NormalDataFactory::id() const
{
    return DesignNet::Constants::DATA_WIDGET_FACTORY_NORMAL;
}

QList<Core::Id> NormalDataFactory::supportedTypes()
{
    QList<Core::Id> types;
    types << DesignNet::Constants::DATA_TYPE_IMAGE;
	types << DesignNet::Constants::DATA_TYPE_MATRIX;
    return types;
}

}
