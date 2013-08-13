#include "boolpropertywidget.h"
#include "../property/boolproperty.h"
#include "designnetconstants.h"
#include <QCheckBox>
namespace DesignNet{
class BoolPropertyWidgetPrivate{
public:
	BoolPropertyWidgetPrivate(BoolPropertyWidget *widget)
		: q(widget)
	{
		m_checkBox = new QCheckBox(widget);
	}
	QCheckBox *m_checkBox;
	BoolPropertyWidget* q;
};
BoolPropertyWidget::BoolPropertyWidget(BoolProperty* prop, QWidget *parent)
	: IPropertyWidget(prop, parent),
	d(new BoolPropertyWidgetPrivate(this))
{
	addWidget(d->m_checkBox);
	d->m_checkBox->setChecked(prop->value());
	connect(d->m_checkBox, SIGNAL(stateChanged ( int )), this, SLOT(onValueChanged(int)));
}

BoolPropertyWidget::~BoolPropertyWidget()
{
	if(d)
	{
		delete d;
		d = 0;
	}
}

void BoolPropertyWidget::onValueChanged( int state )
{
	BoolProperty *property = qobject_cast<BoolProperty*>(m_property);
	if(state == Qt::Checked)
	{
		property->setValue(true);
	}
	else
	{
		property->setValue(false);
	}
}

Core::Id BoolPropertyWidget::id() const
{
	return Constants::PROPERTY_TYPE_BOOL;
}

}
