#include "stdafx.h"
#include "optionproperty.h"
#include <QDebug>


namespace DesignNet
{
OptionProperty::OptionProperty(const QString& id, const QString& name, QObject* parent)
	: Property(id, name, parent)
{

}

OptionProperty::~OptionProperty()
{

}

void OptionProperty::addOption(const QString& text, QVariant data)
{
	m_options[text] = data;
	select(text);
}

void OptionProperty::select(const QString& text)
{
	if (m_options.contains(text))
	{
		m_currentKey = text;
		emit changed();
	}
}

QList<QString> OptionProperty::keys()
{
	return m_options.keys();
}

QVariant OptionProperty::getValue(const QString& key)
{
	return m_options.value(key, QVariant());
}

}
