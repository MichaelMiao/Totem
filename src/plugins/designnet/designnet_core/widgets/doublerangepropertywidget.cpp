#include "doublerangepropertywidget.h"
#include "designnetconstants.h"
#include <QSlider>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QMessageBox>
namespace DesignNet{

class DoubleRangePropertyWidgetPrivate{
public:
	DoubleRangePropertyWidgetPrivate(DoubleRangePropertyWidget* widget)
		: q(widget)
	{

	}
	QHBoxLayout*			m_layout;
	QDoubleSpinBox*			m_spinBox;
	DoubleRangePropertyWidget* q;
};
DoubleRangePropertyWidget::DoubleRangePropertyWidget(DoubleRangeProperty* prop, QWidget *parent)
	: IPropertyWidget(prop, parent),
	d(new DoubleRangePropertyWidgetPrivate(this))
{
	d->m_layout = new QHBoxLayout;
	d->m_layout->setContentsMargins(0, 0, 0, 0);
	d->m_spinBox = new QDoubleSpinBox(parent);
	d->m_spinBox->setMaximum(prop->maximum());
	d->m_spinBox->setMinimum(prop->minimum());
	d->m_layout->addWidget(d->m_spinBox);
	d->m_spinBox->setDecimals(prop->decimals());
	d->m_spinBox->setValue(prop->value());
	connect(d->m_spinBox, SIGNAL(valueChanged ( double)), prop, SLOT(onValueChanged(double)));
	connect(d->m_spinBox, SIGNAL(valueChanged ( double)), this, SLOT(test()));
	addLayout(d->m_layout);
}

DoubleRangePropertyWidget::~DoubleRangePropertyWidget()
{
	if(d)
	{
		delete d;
		d = 0;
	}
}

Core::Id DoubleRangePropertyWidget::id() const
{
	return Constants::PROPERTY_TYPE_DOUBLERANGE;
}

void DoubleRangePropertyWidget::test()
{
}

}
