#ifndef OPTIONPROPERTYWIDGET_H
#define OPTIONPROPERTYWIDGET_H

#include "ipropertywidget.h"
#include "../designnet_core_global.h"
namespace DesignNet{
class OptionPropertyWidgetPrivate;
class OptionProperty;
class DESIGNNET_CORE_EXPORT OptionPropertyWidget : public IPropertyWidget
{
	Q_OBJECT

public:
	OptionPropertyWidget(OptionProperty* prop, QWidget *parent = 0);
	virtual ~OptionPropertyWidget();
	virtual Core::Id id() const;
	void updateData();
public slots:
	void currentIndexChanged ( const QString & text );
private:
	OptionPropertyWidgetPrivate *d;
};

}

#endif // OPTIONPROPERTYWIDGET_H
