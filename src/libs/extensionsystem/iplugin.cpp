#include "iplugin.h"
#include "ipluginprivate.h"
#include "pluginmanager.h"
/*!
    \class ExtensionSystem::IPlugin
    \brief 所有插件的基类
    每个插件包含两个部分：
        一是描述文件，
        另一个就是库（实现了IPlugin接口中的方法）
    插件描述文件是用来解决插件依赖性问题的，
*/
using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;
IPlugin::IPlugin()
    : d(new IPluginPrivate())
{

}

IPlugin::~IPlugin()
{
    PluginManager *pm = PluginManager::instance();
    foreach(QObject *obj, d->m_addedObjectsInReverseOrder)
    {
        pm->removeObject(obj);
    }
    qDeleteAll(d->m_addedObjectsInReverseOrder);
    d->m_addedObjectsInReverseOrder.clear();
    delete d;
    d = 0;
}

PluginSpec *IPlugin::pluginSpec() const
{
    return d->pluginSpec;
}

void IPlugin::addObject(QObject *obj)
{
    PluginManager::instance()->addObject(obj);
}
/*!
    \fn void IPlugin::addAutoReleasedObject(QObject *obj)
    用于在插件池里注册一个obj，通常注册的obj需要从对象池中移出，并且手动
    删除，通过使用addAutoReleasedObject添加到这个pool里的obj会在插件
    实例销毁时被自动移除和删除
*/
void IPlugin::addAutoReleasedObject(QObject *obj)
{
    d->m_addedObjectsInReverseOrder.prepend(obj);
    PluginManager::instance()->addObject(obj);
}

void IPlugin::removeObject(QObject *obj)
{
    PluginManager::instance()->removeObject(obj);
}