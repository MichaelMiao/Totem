#ifndef DESIGNNETCOREPLUGIN_H
#define DESIGNNETCOREPLUGIN_H
#include "extensionsystem/iplugin.h"
namespace Core{
class IMode;
}

namespace DesignNet{
class DesignNetMainWindow;
class DesignNetFormManager;
class DesignNetMode;
class ProcessorFactory;
class PropertyManager;
class DataManager;
class ToolModel;
class DesignNetCorePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    explicit DesignNetCorePlugin(QObject *parent = 0);
    ~DesignNetCorePlugin();
    virtual bool initialize(const QStringList &arguments, QString *errorString);
    virtual void extensionsInitialized();

signals:

public slots:
    void writeSettings();
private:
    DesignNetFormManager*   m_designNetFormMgr;
    DesignNetMode*          m_designNetMode;
    ProcessorFactory*       m_processorFactory;
    PropertyManager*        m_propertyManager;
    DataManager*            m_dataManager;
    ToolModel*              m_model;
};
}
#endif // DESIGNNETCOREPLUGIN_H
