#include "coreplugin.h"
#include "extensionsystem/pluginmanager.h"
#include "mainwindow.h"
#include "editmode.h"
#include "modemanager.h"

#include <QtPlugin>
#include <QDebug>

using namespace Core;
using namespace Core::Internal;

CorePlugin::CorePlugin()
    : m_mainWindow(new MainWindow),
      m_editMode(0)
{

}
CorePlugin::~CorePlugin()
{
    if(m_editMode)
    {
        removeObject(m_editMode);
        delete m_editMode;
    }
    if(m_mainWindow)
    {
        delete m_mainWindow;
        m_mainWindow = 0;
    }
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    const bool success = m_mainWindow->initialize(errorMessage);
    if (success)
    {
		m_editMode = new EditMode;
        addObject(m_editMode);
        ModeManager::activateMode(m_editMode->id());
    }
    return success;
}

void CorePlugin::extensionsInitialized()
{
    m_mainWindow->extensionsInitialized();
}

bool CorePlugin::delayedInitialize()
{
    return true;
}

ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
    m_mainWindow->aboutToShutdown();
    return SynchronousShutdown;
}

void CorePlugin::remoteCommand(const QStringList &, const QStringList &args)
{
}

Q_EXPORT_PLUGIN(CorePlugin)
