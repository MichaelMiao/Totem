#ifndef COREPLUGIN_H
#define COREPLUGIN_H
#include "extensionsystem/iplugin.h"


namespace Core
{
class EditMode;
namespace Internal
{
class MainWindow;
class CorePlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.michaelmiao.totem.PluginInterface")
	Q_INTERFACES(ExtensionSystem::IPlugin)

public:
    CorePlugin();
    ~CorePlugin();

    bool initialize(const QStringList &arguments, QString *errorMessage = 0);
    void extensionsInitialized();
    bool delayedInitialize();
    ShutdownFlag aboutToShutdown();
    void remoteCommand(const QStringList & /* options */, const QStringList &args);
    
signals:
    
public slots:
private:
    MainWindow *m_mainWindow;
    EditMode *m_editMode;
};

}

}

#endif // COREPLUGIN_H
