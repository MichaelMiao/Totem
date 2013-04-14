#ifndef PLUGINMANAGERPRIVATE_H
#define PLUGINMANAGERPRIVATE_H

#include "pluginspec.h"

#include <QObject>
#include <QStringList>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QSettings;
class QTime;
class QTimer;
class QEventLoop;
QT_END_NAMESPACE

namespace ExtensionSystem
{

class PluginManager;
class PluginCollection;

namespace Internal
{

class PluginManagerPrivate : public QObject
{
    Q_OBJECT
public:
    PluginManagerPrivate(PluginManager *pMgr);
    virtual ~PluginManagerPrivate();
    //插件操作
    void setPluginPaths(const QStringList &paths);
    void setSettings(QSettings *settings);
    void setGlobalSettings(QSettings *settings);
    void readSettings();
    void writeSettings();


    void loadPluginsAuto();
    void loadPlugins(QList<PluginSpec *> &queue);//载入插件
    void initPlugins(QList<PluginSpec *> &queue);//初始化插件，调用initialise()函数
    void initPluingsExtension(QList<PluginSpec *> &queue);//extension初始化
    void initPluginsDelayed(); //必须在调用initPluginsExtension之后
    void loadPlugin(PluginSpec *spec, PluginSpec::State destState);


    void shutdown();
    void resolveDependencies();//解决依赖关系
    QList<PluginSpec *> loadQueue();//根据依赖关系排一下载入的顺序

    void initProfiling();
    void profilingReport(const char *what, const PluginSpec *spec = 0);
    void disablePluginIndirectly(PluginSpec *spec);

    //对象池
    void addObject(QObject *obj);
    void removeObject(QObject *obj);


    QString m_extension;//插件文件扩展名,默认为XML格式
    QStringList m_pluginPaths;//插件存放路径
    QSettings *m_settings;
    QSettings *m_globalSettings;
    QStringList m_defaultDisabledPlugins;
    QStringList m_disabledPlugins;
    QStringList m_forceEnabledPlugins;
    QList<QObject *> allObjects;//所有对象
    QHash<QString, PluginCollection *> m_pluginCategories;
    QList<PluginSpec *> m_pluginSpecs;

    //推迟初始化
    QTimer *m_delayedInitializeTimer;
    QList<PluginSpec*> m_delayedInitializeQueue;//推迟初始化列表

    //异步关闭
    QList<PluginSpec* > m_asynchronousPlugins;//要进行异步关闭的插件
    QEventLoop *m_shutdownEventLoop;//用于异步关闭

    //参数列表
    //时间戳，用于统计插架加载时间
    QStringList m_arguments;
    QScopedPointer<QTime> m_profileTimer;
    int m_profileElapsedMS;
    unsigned m_profilingVerbosity;

    //访问选项说明的参数描述
    PluginSpec *pluginForOption(const QString &option, bool *requiresArgument) const;
    PluginSpec *pluginByName(const QString &name) const;

    //用于测试
    static PluginSpec *createSpec();
    static PluginSpecPrivate *privateSpec(PluginSpec *spec);

signals:
    void pluginLoaded(PluginSpec *spec);
    void showMessage(QString message);
public slots:
    void nextDelayedInitialize();
    void asyncShutdownFinished();

private:
    PluginCollection *defaultCollection;
    PluginManager *q;
    void readPluginPaths();
    bool loadQueue(PluginSpec *spec,
            QList<PluginSpec *> &queue,
            QList<PluginSpec *> &circularityCheckQueue);
    void stopAll();
    void deleteAll();
};

}

}

#endif // PLUGINMANAGERPRIVATE_H