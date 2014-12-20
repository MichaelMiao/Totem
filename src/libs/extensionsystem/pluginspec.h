#ifndef PLUGINSPEC_H
#define PLUGINSPEC_H

#include "extensionsystem_global.h"

#include <QList>
#include <QHash>
#include "QString"

QT_BEGIN_NAMESPACE
class QStringList;
QT_END_NAMESPACE

namespace ExtensionSystem
{
namespace Internal
{
    class PluginSpecPrivate;
    class PluginManagerPrivate;
}
class IPlugin;
/*!
  \class ExtensionSystem::PluginDependency
  \brief 该结构体包含了插件名，插件所依赖的插件的兼容版本

*/
struct EXTENSIONSYSTEM_EXPORT PluginDependency
{
    enum Type
    {
        Required,
        Optional
    };
    PluginDependency() : type(Required){}

    QString name;//插件标示
    QString version;//插件必须满足的版本号
    Type type;//这个依赖关系是否是必须的，否则为可选的
    bool operator==(const PluginDependency &other)const
    {
        return name == other.name
                && version == other.version
                && type == other.type;
    }

};

uint qHash(const ExtensionSystem::PluginDependency &value);

struct EXTENSIONSYSTEM_EXPORT PluginArgumentDescription
{
    QString name;
    QString parameter;
    QString description;
};
/*!
  \class ExtensionSystem::PluginSpec
  \brief 包含了插件xml格式的描述文件信息和插件的当前状态
  插件描述PluginSpec的信息由插件载入过程来填充，
  如果发生错误，就可以在插件描述中找到错误描述。
*/
class EXTENSIONSYSTEM_EXPORT PluginSpec
{
public:
    /*!
      \enum ExtensionSystem::PluginSpec::State
      插件在载入过程中的状态变化，提供了在插件载入过程中，
      哪一步发生了错误。
      \value Invalid
             开始点：xml描述文件还没有被载入。
      \value Read
             xml描述文件成功读取，它的信息已经可以通过
             PluginSpec获得。
      \value Resolved
             描述文件中的依赖关系已经被成功找到，并且可以
             通过dependencySpec获得。
      \value Loaded
             插件的库已经被载入，并且创建了插件实例。
             (可以使用plugin())
      \value Initialized
             已经调用了插件实例的initialize()方法，返回成功
      \value Running
             插件依赖关系成功初始化，并且extensionsInitialized成功调用
             整个载入过程完成
      \value Stopped
             插件shutdown, 已经调用了aboutToShutdown方法
      \value Deleted
             删除了插件实例
    */
    enum State { Invalid, Read, Resolved, Loaded,
               Initialized, Running, Stopped, Deleted
               };

    ~PluginSpec();
    //解析xml文件
    QString name() const;
    QString version() const;
    QString compatVersion() const;
    QString creator() const;
    QString copyright() const;
    QString license() const;
    QString description() const;
    QString url() const;
    QString category() const;
    bool isExperimental() const;
    bool isDisabledByDefault() const;
    bool isEnabled() const;
    bool isDisabledIndirectly() const;
    QList<PluginDependency> dependencies() const;
    typedef QList<PluginArgumentDescription> PluginArgumentDescriptions;
    PluginArgumentDescriptions argumentDescriptions() const;

    QString location() const;
    QString filePath() const;
    void setEnabled(bool value);
    void setDisabledByDefault(bool value);
    void setDisabledIndirectly(bool value);

    QStringList arguments() const;
    void setArguments(const QStringList &arguments);
    void addArgument(const QString &argument);

    bool provides(const QString &pluginName, const QString &version) const;

    QHash<PluginDependency, PluginSpec *> dependencySpecs() const;

    // linked plugin instance, valid after 'Loaded' state is reached
    IPlugin *plugin() const;

    // state
    State state() const;
    bool hasError() const;
    QString errorString() const;
private:
    PluginSpec();
    Internal::PluginSpecPrivate *d;
    friend class Internal::PluginManagerPrivate;
    friend class Internal::PluginSpecPrivate;
};

}

#endif // PLUGINSPEC_H