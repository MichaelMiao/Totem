#include "ipropertywidget.h"
#include <QLabel>
#include <QHBoxLayout>
namespace DesignNet{

IPropertyWidget::IPropertyWidget(Property *prop, QWidget *parent) :
    QWidget(parent),
    m_property(prop)
{
    m_label = new QLabel(this);
    m_label->setText(prop->objectName());
    m_layout = new QHBoxLayout;
    m_layout->addWidget(m_label);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(m_layout);
}

Property *IPropertyWidget::property() const
{
    return m_property;
}

void IPropertyWidget::setLabelText(const QString &text) const
{
    m_label->setText(text);
}

void IPropertyWidget::addWidget(QWidget *widget)
{
    m_layout->addWidget(widget);
}

void IPropertyWidget::addLayout(QLayout *layout)
{
    m_layout->addLayout(layout);
}

}
