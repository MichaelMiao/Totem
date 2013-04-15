#include "optionpropertywidget.h"
#include "property/property.h"
#include "property/optionproperty.h"
#include "designnetconstants.h"
#include <QComboBox>
namespace DesignNet{
class OptionPropertyWidgetPrivate
{
public:
	OptionPropertyWidgetPrivate(OptionPropertyWidget *widget);
	QComboBox *m_comboBox;
	OptionPropertyWidget *m_widget;
};

OptionPropertyWidgetPrivate::OptionPropertyWidgetPrivate( OptionPropertyWidget *widget )
	: m_widget(widget)
{
	m_comboBox = new QComboBox(widget);
	
}

OptionPropertyWidget::OptionPropertyWidget(OptionProperty* prop, QWidget *parent)
	: IPropertyWidget(prop, parent),
	d(new OptionPropertyWidgetPrivate(this))
{
	addWidget(d->m_comboBox);
	connect(d->m_comboBox, SIGNAL(currentIndexChanged(const QString &)), 
		this, SLOT(currentIndexChanged(const QString &)));
	updateData();
}

OptionPropertyWidget::~OptionPropertyWidget()
{

}

void OptionPropertyWidget::currentIndexChanged( const QString & text )
{
	OptionProperty *prop = qobject_cast<OptionProperty*>(property());
	if(prop)
	{
		prop->select(text);
	}
}

void OptionPropertyWidget::updateData()
{
	d->m_comboBox->blockSignals(true);
	OptionProperty *prop = qobject_cast<OptionProperty*>(property());
	if(prop)
	{
		QList<QString> keys = prop->keys();
		foreach(QString key, keys)
		{
			d->m_comboBox->addItem(key, prop->getValue(key));
		}
	}
	d->m_comboBox->blockSignals(false);
}

Core::Id OptionPropertyWidget::id() const
{
	return DesignNet::Constants::PROPERTY_TYPE_OPTION;
}

}