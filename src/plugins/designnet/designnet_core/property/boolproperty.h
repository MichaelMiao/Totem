#pragma once
#include "property.h"


namespace DesignNet{

class DESIGNNET_CORE_EXPORT BoolProperty : public Property
{
	Q_OBJECT

public:

	BoolProperty(const QString &id, const QString &name, QObject *parent = 0);
	~BoolProperty();

	void setValue(const bool &value);
	bool value() const;

	void serialize(Utils::XmlSerializer& s) const override;
	void deserialize(Utils::XmlDeserializer& s) override;

public slots:

	void onValueChanged(bool bValue);

private:

	bool m_value;
};

}
