#include "normaldatafactory.h"
#include "designnetconstants.h"
#include "idata.h"
#include "../widgets/imagedatawidget.h"
#include "../widgets/matrixdatawidget.h"
namespace DesignNet{

NormalDataFactory::NormalDataFactory(QObject *parent)
    : IDataWidgetFactory(parent)
{
}

IDataWidget *NormalDataFactory::createWidget(IData *data, QGraphicsItem* parent)
{
	IDataWidget* widget = 0;
	if (!data)
		return 0;
	if(data->id() == DesignNet::Constants::DATA_TYPE_IMAGE)
    {
        widget = new ImageDataWidget(data, parent);
    }
	else if(data->id() == DesignNet::Constants::DATA_TYPE_MATRIX)
	{
		widget = new MatrixDataWidget(data, parent);
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
