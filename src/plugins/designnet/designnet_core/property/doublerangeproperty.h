#ifndef DOUBLERANGEPROPERTY_H
#define DOUBLERANGEPROPERTY_H

#include "property.h"
namespace DesignNet{
class DoubleRangePropertyPrivate;
class DESIGNNET_CORE_EXPORT DoubleRangeProperty : public Property
{
	Q_OBJECT

public:
	DoubleRangeProperty(const QString &id, const QString &name, QObject *parent = 0);
	~DoubleRangeProperty();
	virtual Core::Id typeID() const;
	virtual bool isValid() const;

	void setRange(const double &dMin, const double &dMax);

	void setMinimum(const double &dmin);
	double minimum() const;
	void setMaximum(const double &dmax);
	double maximum() const;
	void setStep(const double &step);
	double step() const;

	void setDecimals ( int prec );
	int decimals() const;

	void setValue(const double &v);
	double value() const;
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;
signals:
	void rangeChanged(double dmin, double dmax);
	void valueChanged ( int value );
public slots:
	void onValueChanged(double value);
private:
	DoubleRangePropertyPrivate *d;
	
};
}


#endif // DOUBLERANGEPROPERTY_H
