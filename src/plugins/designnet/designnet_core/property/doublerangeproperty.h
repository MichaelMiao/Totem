#pragma once
#include "property.h"


namespace DesignNet
{
class DoubleRangeProperty : public Property
{
	Q_OBJECT

public:

	DECLARE_PROPERTY_TYPE(Property_Range)

	DoubleRangeProperty(const QString& id, const QString& name, QObject* parent = 0);
	~DoubleRangeProperty() {}
	void	setRange(const double& dMin, const double& dMax);

	void	setMinimum(const double& dmin);
	void	setMaximum(const double& dmax);
	double	minimum() const;
	double	maximum() const;
	double	step() const;
	void	setStep(const double& step);

	void	setDecimals(int prec);
	int		decimals() const;

	void	setValue(const double& v);
	double	value() const;

	void	serialize(Utils::XmlSerializer& s) const override;

signals:

	void	rangeChanged(double dmin, double dmax);
	void	valueChanged(int value);

public slots:

	void	onValueChanged(double value);

private:


	double	m_value;		//!< double 类型的数据
	double	m_step;			//!< 步长
	double	m_maximum;		//!< 最大值
	double	m_minimum;		//!< 最小值
	int		m_decimals;		//!< 精度
};
}