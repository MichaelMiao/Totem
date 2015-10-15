#include "stdafx.h"
#include "doublerangeproperty.h"
#include "designnet_core_def.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmldeserializer.h"


namespace DesignNet
{
class DoubleRangePropertyPrivate
{
public:

	DoubleRangePropertyPrivate(DoubleRangeProperty* property)
		: q(property)
	{

	}
	
	DoubleRangeProperty* q;
};

DoubleRangeProperty::DoubleRangeProperty(const QString& id, const QString& name, QObject* parent)
	: Property(id, name, parent),
	  d(new DoubleRangePropertyPrivate(this))
{
	d->m_minimum	= 0;
	d->m_maximum	= 100;
	d->m_value		= 0;
	d->m_step		= 1.0f;
	d->m_decimals	= 0;
}

DoubleRangeProperty::~DoubleRangeProperty()
{
	if (d)
	{
		delete d;
		d = 0;
	}
}

void DoubleRangeProperty::setRange(const double& dMin, const double& dMax)
{
	this->blockSignals(true);
	d->m_minimum = dMin;
	d->m_maximum = dMax;
	this->blockSignals(false);
	emit rangeChanged(d->m_minimum, d->m_maximum);
}

void DoubleRangeProperty::setMaximum(const double& dmax)
{
	d->m_maximum = dmax;
	emit rangeChanged(d->m_minimum, d->m_maximum);
}

void DoubleRangeProperty::setMinimum(const double& dmin)
{
	d->m_minimum = dmin;
	emit rangeChanged(d->m_minimum, d->m_maximum);
}

double DoubleRangeProperty::minimum() const
{
	return d->m_minimum;
}

double DoubleRangeProperty::maximum() const
{
	return d->m_maximum;
}

void DoubleRangeProperty::setStep(const double& step)
{
	d->m_step = step;
}

double DoubleRangeProperty::step() const
{
	return d->m_step;
}

void DoubleRangeProperty::onValueChanged(double value)
{
	d->m_value = value;
	emit valueChanged(value);
	emit changed();
}

int DoubleRangeProperty::decimals() const
{
	return d->m_decimals;
}

void DoubleRangeProperty::setDecimals(int prec)
{
	d->m_decimals = prec;
}

void DoubleRangeProperty::setValue(const double& v)
{
	d->m_value = v;
	emit valueChanged(v);
}

double DoubleRangeProperty::value() const
{
	return d->m_value;
}

void DoubleRangeProperty::serialize(Utils::XmlSerializer& s) const
{
	Property::serialize(s);
	s.serialize("value", value());
}

void DoubleRangeProperty::deserialize(Utils::XmlDeserializer& s)
{

}

}
