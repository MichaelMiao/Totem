#include "pluginspec.h"
#include "pluginspecprivate.h"
using namespace ExtensionSystem;
using namespace ExtensionSystem::Internal;

uint ExtensionSystem::qHash(const ExtensionSystem::PluginDependency &value)
{
    return qHash(value.name);
}

PluginSpec::PluginSpec()
    : d(new PluginSpecPrivate(this))
{
}

PluginSpec::~PluginSpec()
{
    if(d)
    {
        delete d;
        d = 0;
    }
}

QString PluginSpec::name() const
{
    return d->name;
}

QString PluginSpec::version() const
{
    return d->version;
}

QString PluginSpec::compatVersion() const
{
    return d->compatVersion;
}

QString PluginSpec::creator() const
{
    return d->creator;
}

QString PluginSpec::copyright() const
{
    return d->copyright;
}

QString PluginSpec::license() const
{
    return d->license;
}

QString PluginSpec::description() const
{
    return d->description;
}

QString PluginSpec::url() const
{
    return d->url;
}

QString PluginSpec::category() const
{
    return d->category;
}

bool PluginSpec::isExperimental() const
{
    return d->experimental;
}

bool PluginSpec::isDisabledByDefault() const
{
    return d->disabledByDefault;
}

bool PluginSpec::isEnabled() const
{
    return d->enabled;
}

bool PluginSpec::isDisabledIndirectly() const
{
    return d->disabledIndirectly;
}

QList<PluginDependency> PluginSpec::dependencies() const
{
    return d->dependencies;
}

PluginSpec::PluginArgumentDescriptions PluginSpec::argumentDescriptions() const
{
    return d->argumentDescriptions;
}

QString PluginSpec::location() const
{
    return d->location;
}

QString PluginSpec::filePath() const
{
    return d->filePath;
}

void PluginSpec::setEnabled(bool value)
{
    d->enabled = value;
}

void PluginSpec::setDisabledByDefault(bool value)
{
    d->disabledByDefault = value;
}

void PluginSpec::setDisabledIndirectly(bool value)
{
    d->disabledIndirectly = value;
}

QStringList PluginSpec::arguments() const
{
    return d->arguments;
}

void PluginSpec::setArguments(const QStringList &arguments)
{
    d->arguments = arguments;
}

void PluginSpec::addArgument(const QString &argument)
{
     d->arguments.push_back(argument);
}

bool PluginSpec::provides(const QString &pluginName, const QString &version) const
{
    return d->provides(pluginName, version);
}

QHash<PluginDependency, PluginSpec *> PluginSpec::dependencySpecs() const
{
    return d->dependencySpecs;
}

IPlugin *PluginSpec::plugin() const
{
    return d->plugin;
}

PluginSpec::State PluginSpec::state() const
{
    return d->state;
}

bool PluginSpec::hasError() const
{
    return d->hasError;
}

QString PluginSpec::errorString() const
{
    return d->errorString;
}