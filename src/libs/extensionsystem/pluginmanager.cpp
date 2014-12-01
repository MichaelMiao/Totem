#include "pluginmanager.h"
#include "pluginmanagerprivate.h"
using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;

//static bool lessThanByPluginName(const PluginSpec *one, const PluginSpec *two)
//{
//    return one->name() < two->name();
//}


PluginManager *PluginManager::m_instance = 0;

PluginManager::PluginManager()
    : d(new PluginManagerPrivate(this))
{
    m_instance = this;
}

PluginManager::~PluginManager()
{
    if(d)
    {
        delete d;
        d = 0;
    }
}

void PluginManager::setSettings(QSettings *settings)
{
    d->setSettings(settings);
}

QSettings *PluginManager::settings() const
{
    return d->m_settings;
}

void PluginManager::setGlobalSettings(QSettings *settings)
{
    d->setGlobalSettings(settings);
}

QSettings *PluginManager::globalSettings() const
{
    return d->m_globalSettings;
}

void PluginManager::writeSettings()
{
    d->writeSettings();
}

//添加obj到对象池当中，不负责内存管理
//emit ObjectAdded()信号
void PluginManager::addObject(QObject *obj)
{
    d->addObject(obj);
}
//先emit aboutToRemoveObject()信号，然后移除obj
void PluginManager::removeObject(QObject *obj)
{
    d->removeObject(obj);
}

QList<QObject *> PluginManager::getAllObjects() const
{
    return d->allObjects;
}

QObject *PluginManager::getObjectByName(const QString &name) const
{
    QReadLocker lock(&m_lock);
    QList<QObject *> all = getAllObjects();
    foreach (QObject *obj, all)
    {
        if (obj->objectName() == name)
            return obj;
    }
    return 0;
}

QObject *PluginManager::getObjectByClassName(const QString &className) const
{
    const QByteArray ba = className.toUtf8();
    QReadLocker lock(&m_lock);
    QList<QObject *> all = getAllObjects();
    foreach (QObject *obj, all)
    {
        if (obj->inherits(ba.constData()))
            return obj;
    }
    return 0;
}

QList<PluginSpec *> PluginManager::loadQueue()
{
    return d->loadQueue();
}

void PluginManager::loadPluginsAuto()
{
    d->loadPluginsAuto();
}

void PluginManager::loadPlugins(QList<PluginSpec *> &queue)
{
    d->loadPlugins(queue);
}

void PluginManager::initPlugins(QList<PluginSpec *> &queue)
{
    d->initPlugins(queue);
}

void PluginManager::initPluingsExtension(QList<PluginSpec *> &queue)
{
    d->initPluingsExtension(queue);
}

void PluginManager::loadPlugin(PluginSpec *spec, PluginSpec::State destState)
{
    d->loadPlugin(spec, destState);
}

void PluginManager::loadPluginCompletely(PluginSpec *spec)
{
    loadPlugin(spec, PluginSpec::Loaded);
    loadPlugin(spec, PluginSpec::Initialized);
    loadPlugin(spec, PluginSpec::Running);
}


PluginManager *PluginManager::instance()
{
    if(!m_instance)
        new PluginManager();
    return m_instance;
}

/*!
    \date: 2012-6-26

    设置插件的搜索路径，递归的搜索，包括子目录
*/
void PluginManager::setPluginPaths(const QStringList &paths)
{
    d->setPluginPaths(paths);
}

QList<PluginSpec *> PluginManager::plugins() const
{
    return d->m_pluginSpecs;
}

QStringList PluginManager::pluginPaths() const
{
    return d->m_pluginPaths;
}

QHash<QString, PluginCollection *> PluginManager::pluginCollections() const
{
    return d->m_pluginCategories;
}

void PluginManager::setFileExtension(const QString &extension)
{
    d->m_extension = extension;
}

QString PluginManager::fileExtension() const
{
    return d->m_extension;
}

void PluginManager::remoteArguments(const QString &serializedArguments)
{
	Q_UNUSED(serializedArguments)
}

void PluginManager::shutdown()
{
    d->shutdown();
}
