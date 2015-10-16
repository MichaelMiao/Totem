#include "stdafx.h"
#include "propertymanager.h"
#include "../widgets/ipropertywidget.h"
#include "coreplugin/icore.h"
#include "extensionsystem/pluginmanager.h"
#include "ipropertywidgetfactory.h"
#include "property.h"
#include <QList>
#include <QDebug>
#include "Utils/totemassert.h"


namespace DesignNet
{
class PropertyManagerPrivate
{
public:
	PropertyManagerPrivate() {}
	QList<IPropertyWidgetFactory*> m_propertyFactories;
};

PropertyManager* PropertyManager::m_instance = 0;
PropertyManager::PropertyManager(QObject* parent) :
	QObject(parent),
	d(new PropertyManagerPrivate)
{
	qDebug() << "PropertyManager";
	connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*)),
	        this, SLOT(objectAdded(QObject*)));
	m_instance = this;
}

PropertyManager::~PropertyManager()
{
}

PropertyManager* PropertyManager::instance()
{
	if (!m_instance)
	{
		m_instance = new PropertyManager;
	}

	return m_instance;
}

IPropertyWidget* PropertyManager::createWidget(Property* property, QWidget* parent)
{
	IPropertyWidget* widget = 0;

	foreach (IPropertyWidgetFactory* propertyFactory, d->m_propertyFactories)
	{
		widget = propertyFactory->createWidget(property, parent);

		if (widget)
			return widget;
	}

	return 0;
}

void PropertyManager::objectAdded(QObject* obj)
{
	IPropertyWidgetFactory* factory = qobject_cast<IPropertyWidgetFactory*>(obj);

	if (!factory)
		return;

	TOTEM_ASSERT(qFind(d->m_propertyFactories, factory) != d->m_propertyFactories.constEnd(), return);

	d->m_propertyFactories.append(factory);
}
}
