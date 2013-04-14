#include "datamanager.h"
#include "data/idatawidgetfactory.h"

#include "extensionsystem/pluginmanager.h"

#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"

#include <QList>
namespace DesignNet {

class DataManagerPrivate
{
public:
    DataManagerPrivate(DataManager *mgr): m_dataManager(mgr){

    }
    QList<IDataWidgetFactory*> m_factories;
    DataManager* m_dataManager;
};

DataManager* DataManager::m_instance = 0;
DataManager::DataManager(QObject *parent) :
    QObject(parent),
    d(new DataManagerPrivate(this))
{
    m_instance = this;
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
}

DataManager::~DataManager()
{
}

DataManager *DataManager::instance()
{
    if(!m_instance)
    {
        m_instance = new DataManager();
    }
    return m_instance;
}

void DataManager::objectAdded(QObject *obj)
{
    IDataWidgetFactory *factory = qobject_cast<IDataWidgetFactory*>(obj);
    if(!factory)
        return;
    if(qFind(d->m_factories, factory) != d->m_factories.constEnd())
    {
        LOGOUT(tr("The property factory has been registered"));
        return;
    }
    d->m_factories.append(factory);
}

IDataWidget *DataManager::createWidget(IData *data, QGraphicsItem *parent)
{
    foreach (IDataWidgetFactory* factory, d->m_factories) {
        IDataWidget* widget = factory->createWidget(data, parent);
        if(widget)
        {
            return widget;
        }
    }
    return 0;
}

}
