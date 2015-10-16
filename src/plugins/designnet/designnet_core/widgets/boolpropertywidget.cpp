#include "stdafx.h"
#include "boolpropertywidget.h"
#include "../property/boolproperty.h"
#include <QCheckBox>


namespace DesignNet
{

BoolPropertyWidget::BoolPropertyWidget(BoolProperty* prop, QWidget* parent)
	: IPropertyWidget(prop, parent)
{
	m_checkBox = new QCheckBox(this);
	addWidget(m_checkBox);
	m_checkBox->setChecked(prop->value());
	connect(m_checkBox, SIGNAL(stateChanged(int)), this, SLOT(onValueChanged(int)));
}

BoolPropertyWidget::~BoolPropertyWidget()
{
}

void BoolPropertyWidget::onValueChanged(int state)
{
	BoolProperty* property = qobject_cast<BoolProperty*>(m_property);

	if (state == Qt::Checked)
	{
		property->setValue(true);
	}

	else
	{
		property->setValue(false);
	}
}
}
