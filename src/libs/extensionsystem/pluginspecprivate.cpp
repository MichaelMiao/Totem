#include "pluginspecprivate.h"
#include "ipluginprivate.h"
#include "pluginspecprivate.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QRegExp>
#include <QtDebug>
#include <QCoreApplication>

#include <QPluginLoader>
typedef QT_PREPEND_NAMESPACE(QPluginLoader) PluginLoader;
using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;
namespace {
    const char PLUGIN[] = "plugin";
    const char PLUGIN_NAME[] = "name";
    const char PLUGIN_VERSION[] = "version";
    const char PLUGIN_COMPATVERSION[] = "compatVersion";
    const char PLUGIN_EXPERIMENTAL[] = "experimental";
    const char CREATOR[] = "creator";
    const char COPYRIGHT[] = "copyright";
    const char LICENSE[] = "license";
    const char DESCRIPTION[] = "description";
    const char URL[] = "url";
    const char CATEGORY[] = "category";
    const char DEPENDENCYLIST[] = "dependencyList";
    const char DEPENDENCY[] = "dependency";
    const char DEPENDENCY_NAME[] = "name";
    const char DEPENDENCY_VERSION[] = "version";
    const char DEPENDENCY_TYPE[] = "type";
    const char DEPENDENCY_TYPE_SOFT[] = "optional";
    const char DEPENDENCY_TYPE_HARD[] = "required";
    const char ARGUMENTLIST[] = "argumentList";
    const char ARGUMENT[] = "argument";
    const char ARGUMENT_NAME[] = "name";
    const char ARGUMENT_PARAMETER[] = "parameter";
}

PluginSpecPrivate::PluginSpecPrivate(PluginSpec *spec)
    : experimental(false),
      disabledByDefault(false),
      enabled(true),
      disabledIndirectly(false),
      plugin(0),
      state(PluginSpec::Invalid),
      hasError(false),
      q(spec)
{
}

bool PluginSpecPrivate::read(const QString &fileName)
{
    //还原、重置
    name
        = version
        = compatVersion
        = creator
        = copyright
        = license
        = description
        = url
        = category
        = location
        = "";
    state = PluginSpec::Invalid;//将状态置为invalid
    hasError = false;
    errorString = "";
    dependencies.clear();
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return reportError(tr("Cannot open file %1 for reading: %2")
                           .arg(QDir::toNativeSeparators(file.fileName()), file.errorString()));
    QFileInfo fileInfo(file);
    location = fileInfo.absolutePath();//所在文件夹路径
    filePath = fileInfo.absoluteFilePath();//完整路径
    QXmlStreamReader reader(&file);
    while (!reader.atEnd()) {
        reader.readNext();
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement:
            readPluginSpec(reader);
            break;
        default:
            break;
        }
    }
    state = PluginSpec::Read;
    return true;
}

static inline QString msgAttributeMissing(const char *elt, const char *attribute)
{
    return QCoreApplication::translate("PluginSpec", "'%1' misses attribute '%2'").arg(QLatin1String(elt), QLatin1String(attribute));
}

static inline QString msgInvalidFormat(const char *content)
{
    return QCoreApplication::translate("PluginSpec", "'%1' has invalid format").arg(content);
}

static inline QString msgInvalidElement(const QString &name)
{
    return QCoreApplication::translate("PluginSpec", "Invalid element '%1'").arg(name);
}

static inline QString msgUnexpectedClosing(const QString &name)
{
    return QCoreApplication::translate("PluginSpec", "Unexpected closing element '%1'").arg(name);
}

static inline QString msgUnexpectedToken()
{
    return QCoreApplication::translate("PluginSpec", "Unexpected token");
}

bool PluginSpecPrivate::provides(const QString &pluginName, const QString &pluginVersion) const
{
    //所给插件名要与当前插件名相等
    if(QString::compare(pluginName, name, Qt::CaseInsensitive) != 0)
    {
        return false;
    }
    //当前插件版本高于所给插件版本，并且所给插件版本版本高于当前插件版本的最低兼容版本。
    return (versionCompare(version, pluginVersion) >= 0) && (versionCompare(compatVersion, pluginVersion) <= 0);
}


bool PluginSpecPrivate::isValidVersion(const QString &version)
{
    return versionRegExp().exactMatch(version);
}

int PluginSpecPrivate::versionCompare(const QString &version1, const QString &version2)
{
    QRegExp reg1 = versionRegExp();
    QRegExp reg2 = versionRegExp();
    if (!reg1.exactMatch(version1))
        return 0;
    if (!reg2.exactMatch(version2))
        return 0;
    int number1;
    int number2;
    for (int i = 0; i < 4; ++i) {
        number1 = reg1.cap(i+1).toInt();
        number2 = reg2.cap(i+1).toInt();
        if (number1 < number2)
            return -1;
        if (number1 > number2)
            return 1;
    }
    return 0;
}

QRegExp &PluginSpecPrivate::versionRegExp()
{
    //开头一个数，紧跟0个或者1个".数",紧跟0个或者1个".数",紧跟0个或者1个"_数"

    static QRegExp reg("([0-9]+)(?:[.]([0-9]+))?(?:[.]([0-9]+))?(?:_([0-9]+))?");
    return reg;
}

void PluginSpecPrivate::disableIndirectlyIfDependencyDisabled()
{
    //在此之前，已经排好序了，他所依赖的插件的状态已经算出来了
    if(!enabled)//如果当前插件不可用，就不用管了
        return;

    if(disabledIndirectly)//如果已经是被用户禁用了，那也不可用
        return;
    QHashIterator<PluginDependency, PluginSpec*> itr(dependencySpecs);
    while(itr.hasNext())
    {
        itr.next();
        if(itr.key().type == PluginDependency::Optional)
            continue;
        PluginSpec *dependencySpec = itr.value();
        if(dependencySpec->isDisabledIndirectly())
        {
            disabledIndirectly = true;
            break;
        }
    }
}

bool PluginSpecPrivate::resolveDependencies(const QList<PluginSpec *> &specs)
{
    if(hasError)//如果有错，就直接返回
        return false;
    if(state == PluginSpec::Resolved)
    {
        state = PluginSpec::Read;//如果是已经解决的，这里重新resolve一次
    }
    if(state != PluginSpec::Read)
    {
        errorString = QCoreApplication::translate("PluginSpec", "Resolving dependencies failed because state != Read");
        hasError = true;
        return false;
    }
    QHash<PluginDependency, PluginSpec*> resolvedDependencies;
    //遍历解析xml文件获得的依赖关系集合
    foreach(const PluginDependency &dependency, dependencies)
    {
        PluginSpec *found = 0;
        foreach(PluginSpec *spec, specs)
        {
            if(spec->provides(dependency.name, dependency.version))
            {
                found = spec;
                break;
            }
        }
        if (!found)//没有找到相符合的插件描述
        {
            //并且这个插件又是必须要加载的
            if(dependency.type == PluginDependency::Required)
            {
                hasError = true;
                if(!errorString.isEmpty())
                    errorString.append(QLatin1Char('\n'));//多个错误连接成一个长的字符串
                errorString.append(QCoreApplication::translate("PluginSpec", "Could not resolve dependency '%1(%2)'")
                    .arg(dependency.name).arg(dependency.version));
            }
            continue;
        }
        //找到了，说明这个依赖关系已经解决，
        //这种解决只是说有这个插件，暂时还没有检查插件的循环依赖问题
        resolvedDependencies.insert(dependency, found);
    }
    if(hasError)
    {
        return false;
    }
    //它的dependecySpecs里存放的是已经解决的依赖关系。
    //并置当前状态置为已经解决的状态。
    dependencySpecs = resolvedDependencies;
    state = PluginSpec::Resolved;
    return true;
}

bool PluginSpecPrivate::loadLibrary()
{
    if(hasError)
        return false;
    if(state != PluginSpec::Resolved)
    {
        if(state == PluginSpec::Loaded)
            return true;
        //没有解决依赖关系
        errorString = QCoreApplication::translate("PluginSpec", "Loading the library failed because state != Resolved");
        hasError = true;
        return false;
    }
#ifdef QT_NO_DEBUG

#ifdef Q_OS_WIN
    QString libName = QString("%1/%2.dll").arg(location).arg(name);
#elif defined(Q_OS_MAC)
    QString libName = QString("%1/lib%2.dylib").arg(location).arg(name);
#else
    QString libName = QString("%1/lib%2.so").arg(location).arg(name);
#endif

#else //Q_NO_DEBUG

#ifdef Q_OS_WIN
    QString libName = QString("%1/%2d.dll").arg(location).arg(name);
#elif defined(Q_OS_MAC)
    QString libName = QString("%1/lib%2_debug.dylib").arg(location).arg(name);
#else
    QString libName = QString("%1/lib%2.so").arg(location).arg(name);
#endif

#endif
    PluginLoader loader(libName);
    if(!loader.load())
    {
        hasError = true;
        errorString = QDir::toNativeSeparators(libName)
                + QString::fromLatin1(": ") + loader.errorString();
        return false;
    }
    IPlugin *pluginObject = qobject_cast<IPlugin*>(loader.instance());
    if(!pluginObject)
    {
        hasError = true;
        errorString = QCoreApplication::translate("PluginSpec", "Plugin is not valid (does not derive from IPlugin)");
        loader.unload();
        return false;
    }
    state = PluginSpec::Loaded;
    plugin = pluginObject;
    plugin->d->pluginSpec = q;
    return true;
}

bool PluginSpecPrivate::initializePlugin()
{
    if(hasError)
        return false;
    if(state != PluginSpec::Loaded)
    {
        if(state == PluginSpec::Initialized)
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Initializing the plugin failed because state != Loaded");
        hasError = true;
        return false;
    }
    if(!plugin)
    {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to initialize");
        hasError = true;
        return false;
    }
    QString err;
    if(!plugin->initialize(arguments, &err))
    {
        errorString = QCoreApplication::translate("PluginSpec", "Plugin initialization failed: %1").arg(err);
        hasError = true;
        return false;
    }
    state = PluginSpec::Initialized;
    return true;
}

bool PluginSpecPrivate::initializeExtensions()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Initialized) //当前状态还没有初始化完成
    {
        if (state == PluginSpec::Running) //如果当前插件正在运行（调用过了initializeExtensions(),并且成功返回）
            return true;
        errorString = QCoreApplication::translate("PluginSpec", "Cannot perform extensionsInitialized because state != Initialized");
        hasError = true;
        return false;
    }
    if (!plugin)//没有相关联的插件
    {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to perform extensionsInitialized");
        hasError = true;
        return false;
    }

    plugin->extensionsInitialized();//调用插件的extensionInitialized()方法
    state = PluginSpec::Running;
    return true;
}

bool PluginSpecPrivate::delayedInitialize()
{
    if (hasError)
        return false;
    if (state != PluginSpec::Running)
    {
        return false;
    }
    if (!plugin)
    {
        errorString = QCoreApplication::translate("PluginSpec", "Internal error: have no plugin instance to perform delayedInitialize");
        hasError = true;
        return false;
    }
    return plugin->delayedInitialize();
}

IPlugin::ShutdownFlag PluginSpecPrivate::stop()
{
    if (!plugin)
        return IPlugin::SynchronousShutdown;
    state = PluginSpec::Stopped;
    return plugin->aboutToShutdown();
}

void PluginSpecPrivate::kill()
{
    if (!plugin)
        return;
    delete plugin;
    plugin = 0;
    state = PluginSpec::Deleted;
}

bool PluginSpecPrivate::reportError(const QString &err)
{
    errorString = err;
    hasError = true;
    return false;
}

void PluginSpecPrivate::readPluginSpec(QXmlStreamReader &reader)
{
    QString element = reader.name().toString();
    if (element != QString(PLUGIN))
    {
        reader.raiseError(QCoreApplication::translate("PluginSpec", "Expected element '%1' as top level element").arg(PLUGIN));
        return;
    }
    name = reader.attributes().value(PLUGIN_NAME).toString();
    if (name.isEmpty())
    {
        reader.raiseError(msgAttributeMissing(PLUGIN, PLUGIN_NAME));
        return;
    }
    version = reader.attributes().value(PLUGIN_VERSION).toString();
    if (version.isEmpty())
    {
        reader.raiseError(msgAttributeMissing(PLUGIN, PLUGIN_VERSION));
        return;
    }
    if (!isValidVersion(version))
    {
        reader.raiseError(msgInvalidFormat(PLUGIN_VERSION));
        return;
    }
    compatVersion = reader.attributes().value(PLUGIN_COMPATVERSION).toString();
    if (!compatVersion.isEmpty() && !isValidVersion(compatVersion))
    {
        reader.raiseError(msgInvalidFormat(PLUGIN_COMPATVERSION));
        return;
    }
    else if (compatVersion.isEmpty())
    {
        compatVersion = version;
    }
    QString experimentalString = reader.attributes().value(PLUGIN_EXPERIMENTAL).toString();
    experimental = (experimentalString.compare(QLatin1String("true"), Qt::CaseInsensitive) == 0);
    if (!experimentalString.isEmpty() && !experimental
            && experimentalString.compare(QLatin1String("false"), Qt::CaseInsensitive) != 0)
    {
        reader.raiseError(msgInvalidFormat(PLUGIN_EXPERIMENTAL));
        return;
    }
    disabledByDefault = experimental;
    enabled = !experimental;
    while (!reader.atEnd())
    {
        reader.readNext();
        switch (reader.tokenType())
        {
        case QXmlStreamReader::StartElement:
            element = reader.name().toString();
            if (element == CREATOR)
                creator = reader.readElementText().trimmed();
            else if (element == COPYRIGHT)
                copyright = reader.readElementText().trimmed();
            else if (element == LICENSE)
                license = reader.readElementText().trimmed();
            else if (element == DESCRIPTION)
                description = reader.readElementText().trimmed();
            else if (element == URL)
                url = reader.readElementText().trimmed();
            else if (element == CATEGORY)
                category = reader.readElementText().trimmed();
            else if (element == DEPENDENCYLIST)
                readDependencies(reader);
            else if (element == ARGUMENTLIST)
                readArgumentDescriptions(reader);
            else
                reader.raiseError(msgInvalidElement(name));
            break;
        case QXmlStreamReader::EndDocument:
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::EndElement:
        case QXmlStreamReader::Characters:
            break;
        default:
            reader.raiseError(msgUnexpectedToken());
            break;
        }
    }
}

void PluginSpecPrivate::readDependencies(QXmlStreamReader &reader)
{
    QString element;
    while (!reader.atEnd()) {
        reader.readNext();
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement:
            element = reader.name().toString();
            if (element == DEPENDENCY) {
                readDependencyEntry(reader);
            } else {
                reader.raiseError(msgInvalidElement(name));
            }
            break;
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::Characters:
            break;
        case QXmlStreamReader::EndElement:
            element = reader.name().toString();
            if (element == DEPENDENCYLIST)
                return;
            reader.raiseError(msgUnexpectedClosing(element));
            break;
        default:
            reader.raiseError(msgUnexpectedToken());
            break;
        }
    }
}

void PluginSpecPrivate::readDependencyEntry(QXmlStreamReader &reader)
{
    PluginDependency dep;
    dep.name = reader.attributes().value(DEPENDENCY_NAME).toString();
    if (dep.name.isEmpty()) {
        reader.raiseError(msgAttributeMissing(DEPENDENCY, DEPENDENCY_NAME));
        return;
    }
    dep.version = reader.attributes().value(DEPENDENCY_VERSION).toString();
    if (!dep.version.isEmpty() && !isValidVersion(dep.version)) {
        reader.raiseError(msgInvalidFormat(DEPENDENCY_VERSION));
        return;
    }
    dep.type = PluginDependency::Required;
    if (reader.attributes().hasAttribute(DEPENDENCY_TYPE)) {
        QString typeValue = reader.attributes().value(DEPENDENCY_TYPE).toString();
        if (typeValue == QLatin1String(DEPENDENCY_TYPE_HARD)) {
            dep.type = PluginDependency::Required;
        } else if (typeValue == QLatin1String(DEPENDENCY_TYPE_SOFT)) {
            dep.type = PluginDependency::Optional;
        } else {
            reader.raiseError(msgInvalidFormat(DEPENDENCY_TYPE));
            return;
        }
    }
    dependencies.append(dep);
    reader.readNext();
    if (reader.tokenType() != QXmlStreamReader::EndElement)
        reader.raiseError(msgUnexpectedToken());
}

void PluginSpecPrivate::readArgumentDescriptions(QXmlStreamReader &reader)
{
    QString element;
    while (!reader.atEnd()) {
        reader.readNext();
        switch (reader.tokenType()) {
        case QXmlStreamReader::StartElement:
            element = reader.name().toString();
            if (element == ARGUMENT) {
                readArgumentDescription(reader);
            } else {
                reader.raiseError(msgInvalidElement(name));
            }
            break;
        case QXmlStreamReader::Comment:
        case QXmlStreamReader::Characters:
            break;
        case QXmlStreamReader::EndElement:
            element = reader.name().toString();
            if (element == ARGUMENTLIST)
                return;
            reader.raiseError(msgUnexpectedClosing(element));
            break;
        default:
            reader.raiseError(msgUnexpectedToken());
            break;
        }
    }
}

void PluginSpecPrivate::readArgumentDescription(QXmlStreamReader &reader)
{
    PluginArgumentDescription arg;
    arg.name = reader.attributes().value(ARGUMENT_NAME).toString();
    if (arg.name.isEmpty()) {
        reader.raiseError(msgAttributeMissing(ARGUMENT, ARGUMENT_NAME));
        return;
    }
    arg.parameter = reader.attributes().value(ARGUMENT_PARAMETER).toString();
    arg.description = reader.readElementText();
    if (reader.tokenType() != QXmlStreamReader::EndElement)
        reader.raiseError(msgUnexpectedToken());
    argumentDescriptions.push_back(arg);
}