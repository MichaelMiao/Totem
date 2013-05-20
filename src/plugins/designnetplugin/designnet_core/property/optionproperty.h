#ifndef OPTIONPROPERTY_H
#define OPTIONPROPERTY_H

#include "property.h"
#include <QHash>
#include <QList>
namespace DesignNet{

class DESIGNNET_CORE_EXPORT OptionProperty : public Property
{
	Q_OBJECT

public:
	OptionProperty(const QString &id, const QString &name, QObject *parent = 0);
	virtual ~OptionProperty();
	virtual Core::Id typeID() const;
	virtual bool isValid() const;
	void addOption(const QString&text, QVariant data = QVariant());
	

	void select(const QString &text);
	QList<QString> keys();
	QVariant getValue(const QString &key);
	QString currentKey() const{return m_currentKey;}
	QVariant value(){return getValue(m_currentKey);}

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;
private:
	QHash<QString, QVariant> m_options;
	QString m_currentKey;
};
}
#endif // OPTIONPROPERTY_H
