#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "extensionsystem_global.h"
#include "aggregation/aggregate.h"
#include "extensionsystem/pluginspec.h"

#include <QObject>
#include <QReadWriteLock>
QT_BEGIN_NAMESPACE
class QTextStream;
class QSettings;
QT_END_NAMESPACE

namespace ExtensionSystem
{

class PluginCollection;

namespace Internal
{
class PluginManagerPrivate;
}
class IPlugin;
class PluginSpec;
class EXTENSIONSYSTEM_EXPORT PluginManager : public QObject
{
    Q_OBJECT
public:
    static PluginManager *instance();//单件模式
    PluginManager();
    virtual ~PluginManager();

    // Settings
    void setSettings(QSettings *settings);
    QSettings *settings() const;
    void setGlobalSettings(QSettings *settings);
    QSettings *globalSettings() const;
    void writeSettings();

    //对象池操作
    void addObject(QObject *obj);
    void removeObject(QObject *obj);
    QList<QObject*> getAllObjects()const;

    template <typename T> QList<T*> getObjects() const
    {
        QReadLocker lock(&m_lock);
        QList<T*> results;
        QList<QObject*> all = getAllObjects();
        QList<T*> result;
        foreach(QObject *obj, all)
        {
            result = Aggregation::query_all<T>(obj);
            if (!result.isEmpty())
            {
                results += result;
            }
        }
        return results;
    }

    template <typename T> T *getObject() const
    {
        QReadLocker lock(&m_lock);
        QList<QObject *> all = getAllObjects();
        T *result = 0;
        foreach (QObject *obj, all)
        {
            if ((result = Aggregation::query<T>(obj)) != 0)
                break;
        }
        return result;
    }
    QObject *getObjectByName(const QString &name) const;
    QObject *getObjectByClassName(const QString &className) const;

    //插件操作
    QList<PluginSpec *> loadQueue();

    void loadPluginsAuto();
    void loadPlugins(QList<PluginSpec *> &queue);//载入插件
    void initPlugins(QList<PluginSpec *> &queue);//初始化插件，调用initialise()函数
    void initPluingsExtension(QList<PluginSpec *> &queue);//extension初始化
    void loadPlugin(PluginSpec *spec, PluginSpec::State destState);
    void loadPluginCompletely(PluginSpec *spec);

    QStringList pluginPaths() const;
    void setPluginPaths(const QStringList &paths);
    QList<PluginSpec *> plugins() const;
    QHash<QString, PluginCollection *> pluginCollections() const;
    void setFileExtension(const QString &extension);
    QString fileExtension() const;
    bool hasError() const;
    Internal::PluginManagerPrivate *d;

signals:
    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);

    void pluginsChanged();

public slots:
    void remoteArguments(const QString &serializedArguments);
    void shutdown();
private:
    static PluginManager *m_instance;

    mutable QReadWriteLock m_lock;

    friend class Internal::PluginManagerPrivate;
};

}
#endif // PLUGINMANAGER_H