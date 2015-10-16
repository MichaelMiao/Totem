#ifndef OPTIONPROPERTY_H
#define OPTIONPROPERTY_H

#include "property.h"
namespace DesignNet
{

class OptionProperty : public Property
{
	Q_OBJECT

public:

	DECLARE_PROPERTY_TYPE(DesignNet::Property_Bool)

	OptionProperty(const QString& id, const QString& name, QObject* parent = 0);
	virtual ~OptionProperty();
	
	void		addOption(const QString& text, QVariant data = QVariant());
	void		select(const QString& text);
	QVariant	value() { return getValue(m_currentKey); }
	QVariant	getValue(const QString& key);
	QString		currentKey() const { return m_currentKey; }
	QList<QString> keys();

private:

	QHash<QString, QVariant> m_options;
	QString m_currentKey;
};
}
#endif // OPTIONPROPERTY_H
