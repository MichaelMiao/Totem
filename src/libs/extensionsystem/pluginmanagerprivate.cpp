#include "pluginmanagerprivate.h"
#include "pluginmanager.h"
#include "pluginspec.h"
#include "pluginspecprivate.h"
#include "iplugin.h"
#include "plugincollection.h"

#include <QEventLoop>
#include <QDateTime>
#include <QDir>
#include <QMetaProperty>
#include <QSettings>
#include <QTextStream>
#include <QTime>
#include <QWriteLocker>
#include <QtDebug>
#include <QTimer>
#include "..\src\gui\dialogs\qmessagebox.h"
using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;

static const char C_DEFAULT_EXTENSION[] = "xml";
static const char C_IGNORED_PLUGINS[] = "Plugins/Ignored";
static const char C_FORCEENABLED_PLUGINS[] = "Plugins/ForceEnabled";
static const int DELAYED_INITIALIZE_INTERVAL = 20; // ms

typedef QList<ExtensionSystem::PluginSpec *> PluginSpecSet;

static bool lessThanByPluginName(const PluginSpec *one, const PluginSpec *two)
{
    return one->name() < two->name();
}


enum { debugLeaks = 0 };
PluginManagerPrivate::PluginManagerPrivate(PluginManager *pMgr):
    m_extension(QLatin1String(C_DEFAULT_EXTENSION)),
    m_settings(0),
    m_globalSettings(0),
    q(pMgr),
    m_shutdownEventLoop(0),
    m_profileElapsedMS(0),
    m_profilingVerbosity(0),
    m_delayedInitializeTimer(0)
{
}

PluginManagerPrivate::~PluginManagerPrivate()
{
    qDeleteAll(m_pluginSpecs);
    qDeleteAll(m_pluginCategories);
    if(m_shutdownEventLoop)
    {
        m_shutdownEventLoop->deleteLater();
    }
}

void PluginManagerPrivate::setPluginPaths(const QStringList &paths)
{
    m_pluginPaths = paths;
    readSettings();
    readPluginPaths();
}

void PluginManagerPrivate::setSettings(QSettings *s)
{
    if (m_settings)
    {
        delete m_settings;
        m_settings = 0;
    }
    m_settings = s;
    if (m_settings)
        m_settings->setParent(this);
}

void PluginManagerPrivate::setGlobalSettings(QSettings *settings)
{
    if(m_globalSettings)
    {
        delete m_globalSettings;
        m_globalSettings = 0;
    }
    m_globalSettings = settings;
    if(m_globalSettings)
        m_globalSettings->setParent(this);
}

void PluginManagerPrivate::readSettings()
{
    if (m_globalSettings)
    {
        m_defaultDisabledPlugins = m_globalSettings->value(QLatin1String(C_IGNORED_PLUGINS)).toStringList();
    }
    if (m_settings)
    {
        m_disabledPlugins = m_settings->value(QLatin1String(C_IGNORED_PLUGINS)).toStringList();
        m_forceEnabledPlugins = m_settings->value(QLatin1String(C_FORCEENABLED_PLUGINS)).toStringList();
    }
}

void PluginManagerPrivate::writeSettings()
{
    if (!m_settings)
        return;
    QStringList tempDisabledPlugins;
    QStringList tempForceEnabledPlugins;
    foreach(PluginSpec *spec, m_pluginSpecs)
    {
        if (!spec->isDisabledByDefault() && !spec->isEnabled())
            tempDisabledPlugins.append(spec->name());
        if (spec->isDisabledByDefault() && spec->isEnabled())
            tempForceEnabledPlugins.append(spec->name());
    }

    m_settings->setValue(QLatin1String(C_IGNORED_PLUGINS), tempDisabledPlugins);
    m_settings->setValue(QLatin1String(C_FORCEENABLED_PLUGINS), tempForceEnabledPlugins);

}

void PluginManagerPrivate::loadPluginsAuto()
{
    QList<PluginSpec *> queue = loadQueue();
    loadPlugins(queue);
    initPlugins(queue);
    initPluingsExtension(queue);
    initPluginsDelayed();
}

void PluginManagerPrivate::loadPlugins(QList<PluginSpec *> &queue)
{
    //按顺序先载入
    foreach (PluginSpec *spec, queue)
    {
        loadPlugin(spec, PluginSpec::Loaded);
    }
}

void PluginManagerPrivate::initPlugins(QList<PluginSpec *> &queue)
{
    //初始化
    foreach (PluginSpec *spec, queue)
    {
        loadPlugin(spec, PluginSpec::Initialized);
    }
}

void PluginManagerPrivate::initPluingsExtension(QList<PluginSpec *> &queue)
{
    //initialize和extensionInitialize都调用完毕后，
    //要调用delayedInitialize()，这里先放到队列里面
    QListIterator<PluginSpec *> it(queue);
    it.toBack();
    while (it.hasPrevious())
    {
        PluginSpec *spec = it.previous();
        loadPlugin(spec, PluginSpec::Running);
        if (spec->state() == PluginSpec::Running)
            m_delayedInitializeQueue.append(spec);
    }
    emit q->pluginsChanged();
}

void PluginManagerPrivate::initPluginsDelayed()
{
    //设置时间间隔，这里是20ms调用一个插件的delayedInitialize()
    m_delayedInitializeTimer = new QTimer;
    m_delayedInitializeTimer->setInterval(DELAYED_INITIALIZE_INTERVAL);
    m_delayedInitializeTimer->setSingleShot(true);
    connect(m_delayedInitializeTimer, SIGNAL(timeout()),
            this, SLOT(nextDelayedInitialize()));
    m_delayedInitializeTimer->start();
}

void PluginManagerPrivate::loadPlugin(PluginSpec *spec, PluginSpec::State destState)
{
    if(spec->hasError() || spec->state() != destState - 1)
        return ;
    //不载入不可用的插件
    if((spec->isDisabledIndirectly() || !spec->isEnabled()))
        return;
    switch (destState)
    {
    case PluginSpec::Running:
        profilingReport(">initializeExtensions", spec);
        spec->d->initializeExtensions();
        profilingReport("<initializeExtensions", spec);
        return;
    case PluginSpec::Deleted:
        profilingReport(">delete", spec);
        spec->d->kill();
        profilingReport("<delete", spec);
        return;
    default:
        break;
    }
    //检查他所依赖的插件是否已经处于目标状态
    QHashIterator<PluginDependency, PluginSpec *> it(spec->dependencySpecs());
    while (it.hasNext())
    {
        it.next();
        if (it.key().type == PluginDependency::Optional)
            continue;
        PluginSpec *depSpec = it.value();
        if (depSpec->state() != destState)
        {
            spec->d->hasError = true;
            spec->d->errorString =
                PluginManager::tr("Cannot load plugin because dependency failed to load: %1(%2)\nReason: %3")
                    .arg(depSpec->name()).arg(depSpec->version()).arg(depSpec->errorString());
            return;
        }
    }
    //说明所依赖的插件已经为目标状态
    switch (destState)
    {
    case PluginSpec::Loaded:
        profilingReport(">loadLibrary", spec);
        if(!spec->d->loadLibrary())
		{
			QMessageBox::warning(NULL, spec->errorString(), spec->filePath(), QMessageBox::Yes);
		}
        profilingReport("<loadLibrary", spec);
        break;
    case PluginSpec::Initialized:
        profilingReport(">initializePlugin", spec);
        spec->d->initializePlugin();
        profilingReport("<initializePlugin", spec);
        break;
    case PluginSpec::Stopped:
        profilingReport(">stop", spec);//如果是停止就要看是要异步，还是同步
        if (spec->d->stop() == IPlugin::AsynchronousShutdown)
        {//异步就要添加
            m_asynchronousPlugins << spec;
            connect(spec->plugin(), SIGNAL(asynchronousShutdownFinished()),
                    this, SLOT(asyncShutdownFinished()));
        }
        profilingReport("<stop", spec);
        break;
    default:
        break;
    }
}

void PluginManagerPrivate::shutdown()
{
    stopAll();//停止所有插件
    //如果有异步停止，那么就等所有的异步关闭插件都停止位置
    //这里用QEventLoop进行事件循环
    if (!m_asynchronousPlugins.isEmpty())
    {
        m_shutdownEventLoop = new QEventLoop;
        m_shutdownEventLoop->exec();
    }
    deleteAll();//所有插件停止后，就可以删除插件
    if (!allObjects.isEmpty())
    {
        qDebug() << "There are" << allObjects.size() << "objects left in the plugin manager pool: " << allObjects;
    }
}
/*!
  \fn asyncShutdownFinished()
  \brief 异步关闭完成的槽

*/
void PluginManagerPrivate::asyncShutdownFinished()
{
    IPlugin *plugin = qobject_cast<IPlugin *>(sender());
    Q_ASSERT(plugin);
    m_asynchronousPlugins.removeAll(plugin->pluginSpec());
    if (m_asynchronousPlugins.isEmpty())
        m_shutdownEventLoop->exit();
}

void PluginManagerPrivate::readPluginPaths()
{
    qDeleteAll(m_pluginCategories);
    qDeleteAll(m_pluginSpecs);
    m_pluginSpecs.clear();
    m_pluginCategories.clear();

    QStringList specFiles;
    QStringList searchPaths = m_pluginPaths;
    while (!searchPaths.isEmpty())
    {
        const QDir dir(searchPaths.takeFirst());
        const QString pattern = QLatin1String("*.") + m_extension;
        const QFileInfoList files = dir.entryInfoList(QStringList(pattern), QDir::Files);
        foreach (const QFileInfo &file, files)
            specFiles << file.absoluteFilePath();
        const QFileInfoList dirs = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
        foreach (const QFileInfo &subdir, dirs)
            searchPaths << subdir.absoluteFilePath();
    }
    defaultCollection = new PluginCollection(QString());
    m_pluginCategories.insert("", defaultCollection);

    foreach (const QString &specFile, specFiles)
    {
        PluginSpec *spec = new PluginSpec;
        spec->d->read(specFile);

        PluginCollection *collection = 0;
        //查找插件分类，没有就添加一个分类
        // find correct plugin collection or create a new one
        if (m_pluginCategories.contains(spec->category()))
            collection = m_pluginCategories.value(spec->category());
        else
        {
            collection = new PluginCollection(spec->category());
            m_pluginCategories.insert(spec->category(), collection);
        }
        if (m_defaultDisabledPlugins.contains(spec->name()))
        {
            spec->setDisabledByDefault(true);
            spec->setEnabled(false);
        }
        if (spec->isDisabledByDefault() && m_forceEnabledPlugins.contains(spec->name()))
            spec->setEnabled(true);
        if (!spec->isDisabledByDefault() && m_disabledPlugins.contains(spec->name()))
            spec->setEnabled(false);

        collection->addPlugin(spec);
        m_pluginSpecs.append(spec);
    }
    resolveDependencies();
    qSort(m_pluginSpecs.begin(), m_pluginSpecs.end(), lessThanByPluginName);
    emit q->pluginsChanged();
}

/*!
  \fn resolveDependencies()
  \brief 解决依赖关系

*/
void PluginManagerPrivate::resolveDependencies()
{
    //解决了是否有所依赖的插件的问题
    foreach(PluginSpec *spec, m_pluginSpecs)
    {
        spec->d->resolveDependencies(m_pluginSpecs);
    }
    //先loadQueue()排一下载入的顺序，然后每个插件根据自己所依赖的插件的状态，
    //修改自己的状态，如果所依赖的插件不可用，那么自己也不可用
    foreach(PluginSpec *spec, loadQueue())
    {
        spec->d->disableIndirectlyIfDependencyDisabled();
    }
}

/*!
  \fn   loadQueue()
  \brief 最终queue就是排好序的，从头到尾依次载入就可以了。
  借助circularityCheckQueue来排序，将最终解决的放入到queue中，退出递归时就可以放入queue了

*/

QList<PluginSpec *> PluginManagerPrivate::loadQueue()
{
    QList<PluginSpec*> queue;
    foreach(PluginSpec *spec, m_pluginSpecs)
    {
        QList<PluginSpec*> circularityCheckQueue;
        loadQueue(spec, queue, circularityCheckQueue);
    }
    return queue;
}

bool PluginManagerPrivate::loadQueue(PluginSpec *spec, QList<PluginSpec *> &queue,
                                     QList<PluginSpec *> &circularityCheckQueue)
{
    if(queue.contains(spec))
        return true;
    //检查循环依赖
    if(circularityCheckQueue.contains(spec))//循环依赖检查队列中包含这个插件描述，输出错误信息
    {
        spec->d->hasError = true;
        spec->d->errorString = PluginManager::tr("Circular dependency detected:\n");
        int index = circularityCheckQueue.indexOf(spec);
        for (int i = index; i < circularityCheckQueue.size(); ++i)
        {
            spec->d->errorString.append(PluginManager::tr("%1(%2) depends on\n")
                .arg(circularityCheckQueue.at(i)->name()).arg(circularityCheckQueue.at(i)->version()));
        }
        spec->d->errorString.append(PluginManager::tr("%1(%2)").arg(spec->name()).arg(spec->version()));
        return false;
    }
    circularityCheckQueue.append(spec);//添加当前插件描述到循环里来检查队列里面
    // 检查当前插件描述是否是无效的或者
    if (spec->state() == PluginSpec::Invalid || spec->state() == PluginSpec::Read)
    {
        queue.append(spec);
        return false;
    }

    // add dependencies
    foreach (PluginSpec *depSpec, spec->dependencySpecs()) {
        if (!loadQueue(depSpec, queue, circularityCheckQueue)) {
            spec->d->hasError = true;
            spec->d->errorString =
                PluginManager::tr("Cannot load plugin because dependency failed to load: %1(%2)\nReason: %3")
                    .arg(depSpec->name()).arg(depSpec->version()).arg(depSpec->errorString());
            return false;
        }
    }
    // 将自己放到queue队列当中
    queue.append(spec);
    return true;
}

void PluginManagerPrivate::initProfiling()
{
    if (m_profileTimer.isNull())
    {
        m_profileTimer.reset(new QTime);
        m_profileTimer->start();
        m_profileElapsedMS = 0;
        qDebug("Profiling started");
    }
    else
    {
        m_profilingVerbosity++;
    }
}
/*!
    \fn void PluginManagerPrivate::profilingReport(const char *what, const PluginSpec *spec)
    \brief 性能报表

*/
void PluginManagerPrivate::profilingReport(const char *what, const PluginSpec *spec)
{
    if(!m_profileTimer.isNull())
    {
        //自从上次start到现在经历了多长时间
        const int absoluteElapsedMS = m_profileTimer->elapsed();
        const int elapsedMS = absoluteElapsedMS - m_profileElapsedMS;
        m_profileElapsedMS = absoluteElapsedMS;
        if(spec)
        {
            //占22个字符的位置，“-”靠近左边显示，“22”长度
            qDebug("%-22s %-22s %8dms (%8dms)", what, qPrintable(spec->name()), absoluteElapsedMS, elapsedMS);
        }
        else
        {
            qDebug("%-45s %8dms (%8dms)", what, absoluteElapsedMS, elapsedMS);
        }
    }
}

void PluginManagerPrivate::disablePluginIndirectly(PluginSpec *spec)
{
    spec->d->disabledIndirectly = true;
}

void PluginManagerPrivate::addObject(QObject *obj)
{
    {
        QWriteLocker lock(&(q->m_lock));
        if (obj == 0)
        {
            qWarning() << "PluginManagerPrivate::addObject(): trying to add null object";
            return;
        }
        if (allObjects.contains(obj))
        {
            qWarning() << "PluginManagerPrivate::addObject(): trying to add duplicate object";
            return;
        }
        if (debugLeaks)
            qDebug() << "PluginManagerPrivate::addObject" << obj << obj->objectName();
        if (m_profilingVerbosity && !m_profileTimer.isNull())
        {
            //用于统计插件加载、初始化时间
            const int absoluteElapsedMS = m_profileTimer->elapsed();
            qDebug("  %-43s %8dms", obj->metaObject()->className(), absoluteElapsedMS);
        }
        allObjects.append(obj);
    }
    emit q->objectAdded(obj);
}

void PluginManagerPrivate::removeObject(QObject *obj)
{
    //从对象池中移除obj

    if (obj == 0)
    {
        qWarning() << "PluginManagerPrivate::removeObject(): trying to remove null object";
        return;
    }

    if (!allObjects.contains(obj))
    {
        qWarning() << "PluginManagerPrivate::removeObject(): object not in list:"
            << obj << obj->objectName();
        return;
    }
    if (debugLeaks)
        qDebug() << "PluginManagerPrivate::removeObject" << obj << obj->objectName();

    emit q->aboutToRemoveObject(obj);
    QWriteLocker lock(&(q->m_lock));
    allObjects.removeAll(obj);
}

PluginSpec *PluginManagerPrivate::pluginForOption(const QString &option, bool *requiresArgument) const
{
    typedef PluginSpec::PluginArgumentDescriptions PluginArgumentDescriptions;

    *requiresArgument = false;
    const PluginSpecSet::const_iterator pcend = m_pluginSpecs.constEnd();
    for (PluginSpecSet::const_iterator pit = m_pluginSpecs.constBegin(); pit != pcend; ++pit)
    {
        PluginSpec *ps = *pit;
        const PluginArgumentDescriptions pargs = ps->argumentDescriptions();
        if (!pargs.empty())
        {
            const PluginArgumentDescriptions::const_iterator acend = pargs.constEnd();
            for (PluginArgumentDescriptions::const_iterator ait = pargs.constBegin(); ait != acend; ++ait)
            {
                if (ait->name == option)
                {
                    *requiresArgument = !ait->parameter.isEmpty();
                    return ps;
                }
            }
        }
    }
    return 0;
}

PluginSpec *PluginManagerPrivate::pluginByName(const QString &name) const
{
    foreach (PluginSpec *spec, m_pluginSpecs)
        if (spec->name() == name)
            return spec;
    return 0;
}

PluginSpec *PluginManagerPrivate::createSpec()
{
    return new PluginSpec();
}

PluginSpecPrivate *PluginManagerPrivate::privateSpec(PluginSpec *spec)
{
    return spec->d;
}

void PluginManagerPrivate::nextDelayedInitialize()
{
    while(!m_delayedInitializeQueue.isEmpty())
    {
        PluginSpec *spec = m_delayedInitializeQueue.takeFirst();
        profilingReport(">delayedInitialize", spec);
        bool delay = spec->d->delayedInitialize();
        profilingReport("<delayedInitialize", spec);
        if(delay)
            break;
    }
    if(m_delayedInitializeQueue.isEmpty())
    {
        delete m_delayedInitializeTimer;
        m_delayedInitializeTimer = 0;
    }
    else
    {
        m_delayedInitializeTimer->start();
    }
}

void PluginManagerPrivate::stopAll()
{
    if(m_delayedInitializeTimer && m_delayedInitializeTimer->isActive())
    {
        m_delayedInitializeTimer->stop();
        delete m_delayedInitializeTimer;
        m_delayedInitializeTimer = 0;
    }
    QList<PluginSpec *> queue = loadQueue();
    foreach(PluginSpec *spec, queue)
    {
        loadPlugin(spec, PluginSpec::Stopped);
    }
}

void PluginManagerPrivate::deleteAll()
{
    QList<PluginSpec *> queue = loadQueue();
    QListIterator<PluginSpec *> itr(queue);
    itr.toBack();
    while(itr.hasPrevious())
    {
        loadPlugin(itr.previous(), PluginSpec::Deleted);
    }
}