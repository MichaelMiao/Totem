#ifndef PLUGINSPECPRIVATE_H
#define PLUGINSPECPRIVATE_H

#include "pluginspec.h"
#include "iplugin.h"

#include <QObject>
#include <QStringList>
#include <QXmlStreamReader>

namespace ExtensionSystem
{
class IPlugin;
class PluginManager;
namespace Internal
{

class PluginSpecPrivate : public QObject
{
    Q_OBJECT

public:
    PluginSpecPrivate(PluginSpec *spec);
    bool read(const QString &fileName);
    //校验插件是否兼容
    bool provides(const QString &pluginName, const QString &pluginVersion) const;
    bool resolveDependencies(const QList<PluginSpec *> &specs);
    bool loadLibrary();
    bool initializePlugin();
    bool initializeExtensions();
    bool delayedInitialize();
    IPlugin::ShutdownFlag stop();
    void kill();

    QString name;
    QString version;
    QString compatVersion;
    bool experimental;
    bool disabledByDefault;
    QString creator;
    QString copyright;
    QString license;
    QString description;
    QString url;
    QString category;
    QList<PluginDependency> dependencies;
    bool enabled;
    bool disabledIndirectly;

    QString location;
    QString filePath;
    QStringList arguments;

    QHash<PluginDependency, PluginSpec *> dependencySpecs;
    PluginSpec::PluginArgumentDescriptions argumentDescriptions;
    IPlugin *plugin;

    PluginSpec::State state;
    bool hasError;
    QString errorString;

    static bool isValidVersion(const QString &version);
    static int versionCompare(const QString &version1, const QString &version2);
    //如果依赖的插件不可用，那么就置当前插件不可用（Indirectly）
    void disableIndirectlyIfDependencyDisabled();
private:
    PluginSpec *q;

    bool reportError(const QString &err);
    void readPluginSpec(QXmlStreamReader &reader);
    void readDependencies(QXmlStreamReader &reader);
    void readDependencyEntry(QXmlStreamReader &reader);
    void readArgumentDescriptions(QXmlStreamReader &reader);
    void readArgumentDescription(QXmlStreamReader &reader);

    static QRegExp &versionRegExp();
};

}

}

#endif // PLUGINSPECPRIVATE_H