#include "stdafx.h"
#include "doublerangeproperty.h"
#include "designnet_core_def.h"


namespace DesignNet
{
DoubleRangeProperty::DoubleRangeProperty(const QString& id, const QString& name, QObject* parent)
	: Property(id, name, parent), m_minimum(0), m_maximum(100), m_value(0), m_step(1.0f), m_decimals(0)
{
}

void DoubleRangeProperty::setRange(const double& dMin, const double& dMax)
{
	this->blockSignals(true);
	m_minimum = dMin;
	m_maximum = dMax;
	this->blockSignals(false);
	emit rangeChanged(m_minimum, m_maximum);
}

void DoubleRangeProperty::setMaximum(const double& dmax)
{
	m_maximum = dmax;
	emit rangeChanged(m_minimum, m_maximum);
}

void DoubleRangeProperty::setMinimum(const double& dmin)
{
	m_minimum = dmin;
	emit rangeChanged(m_minimum, m_maximum);
}

double DoubleRangeProperty::minimum() const
{
	return m_minimum;
}

double DoubleRangeProperty::maximum() const
{
	return m_maximum;
}

void DoubleRangeProperty::setStep(const double& step)
{
	m_step = step;
}

double DoubleRangeProperty::step() const
{
	return m_step;
}

void DoubleRangeProperty::onValueChanged(double value)
{
	m_value = value;
	emit valueChanged(value);
	emit changed();
}

int DoubleRangeProperty::decimals() const
{
	return m_decimals;
}

void DoubleRangeProperty::setDecimals(int prec)
{
	m_decimals = prec;
}

void DoubleRangeProperty::setValue(const double& v)
{
	m_value = v;
	emit valueChanged(v);
}

double DoubleRangeProperty::value() const
{
	return m_value;
}

void DoubleRangeProperty::serialize(Utils::XmlSerializer& s) const
{
	Property::serialize(s);
	s.serialize("value", m_value);
}

}
