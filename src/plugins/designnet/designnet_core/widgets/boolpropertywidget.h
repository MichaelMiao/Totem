#ifndef BOOLPROPERTYWIDGET_H
#define BOOLPROPERTYWIDGET_H

#include "ipropertywidget.h"
namespace DesignNet{
class BoolPropertyWidgetPrivate;
class BoolProperty;
class DESIGNNET_CORE_EXPORT BoolPropertyWidget : public IPropertyWidget
{
	Q_OBJECT

public:
	BoolPropertyWidget(BoolProperty* prop, QWidget *parent = 0);
	~BoolPropertyWidget();
	virtual Core::Id id() const;
public slots:
	void onValueChanged(int state);
private:
	BoolPropertyWidgetPrivate *d;
};

}

#endif // BOOLPROPERTYWIDGET_H
