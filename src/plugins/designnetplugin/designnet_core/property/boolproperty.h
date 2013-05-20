#ifndef BOOLOPTION_H
#define BOOLOPTION_H

#include "property.h"
namespace DesignNet{

class DESIGNNET_CORE_EXPORT BoolProperty : public Property
{
	Q_OBJECT

public:
	BoolProperty(const QString &id, const QString &name, QObject *parent = 0);
	~BoolProperty();
	virtual Core::Id typeID() const;
	virtual bool isValid() const;
	void setValue(const bool &value);
	bool value() const;
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;
public slots:
	void onValueChanged(bool bValue);
private:
	bool m_value;
};

}

#endif // BOOLOPTION_H
