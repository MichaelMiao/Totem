#include "messagemanager.h"
#include "messageoutputwindow.h"

#include <extensionsystem/pluginmanager.h>

#include <QStatusBar>
#include <QApplication>

using namespace Core;

MessageManager *MessageManager::m_instance = 0;

MessageManager::MessageManager()
    : m_messageOutputWindow(0)
{
    m_instance = this;
}

MessageManager::~MessageManager()
{
    if (m_messageOutputWindow) {
        ExtensionSystem::PluginManager::instance()->removeObject(m_messageOutputWindow);
        delete m_messageOutputWindow;
    }

    m_instance = 0;
}

void MessageManager::init()
{
    m_messageOutputWindow = new Internal::MessageOutputWindow;
    ExtensionSystem::PluginManager::instance()->addObject(m_messageOutputWindow);
}

void MessageManager::showOutputPane()
{
    if (m_messageOutputWindow)
        m_messageOutputWindow->popup(false);
}

void MessageManager::printToOutputPane(const QString &text, bool bringToForeground)
{
    if (!m_messageOutputWindow)
        return;
    if (bringToForeground)
        m_messageOutputWindow->popup(false);
    m_messageOutputWindow->append(text + QLatin1Char('\n'));
}

void MessageManager::printToOutputPanePopup(const QString &text)
{
    printToOutputPane(text, true);
}

void MessageManager::printToOutputPane(const QString &text)
{
    printToOutputPane(text, false);
}

