#ifndef IPLUGINPRIVATE_H
#define IPLUGINPRIVATE_H

#include "iplugin.h"

#include <QString>
namespace ExtensionSystem
{

class PluginManager;
class PluginSpec;
namespace Internal
{
class IPluginPrivate
{
public:
    IPluginPrivate();
    PluginSpec *pluginSpec;
    QList<QObject *> m_addedObjectsInReverseOrder;
};
} //namespace Internal

} //namespace ExtensionSystem
#endif // IPLUGINPRIVATE_H