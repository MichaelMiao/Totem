#ifndef DOUBLERANGEPROPERTYWIDGET_H
#define DOUBLERANGEPROPERTYWIDGET_H

#include "ipropertywidget.h"
#include "../property/doublerangeproperty.h"
namespace DesignNet{
class DoubleRangePropertyWidgetPrivate;
class DESIGNNET_CORE_EXPORT DoubleRangePropertyWidget : public IPropertyWidget
{
	Q_OBJECT

public:
	DoubleRangePropertyWidget(DoubleRangeProperty* prop, QWidget *parent = 0);
	~DoubleRangePropertyWidget();
	virtual Core::Id id() const;
public slots:
	void test();
private:
	DoubleRangePropertyWidgetPrivate* d;
};
}
#endif // DOUBLERANGEPROPERTYWIDGET_H
