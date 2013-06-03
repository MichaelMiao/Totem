#include "icore.h"
#include "app/app_version.h"
#include "mainwindow.h"
#include "messagemanager.h"
#include "modewidget.h"
#include "utils/XML/xmlserializablefactory.h"

#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QStatusBar>

using namespace Core;
using namespace Utils;
ICore * ICore::m_instance = 0;
Internal::MainWindow *ICore::m_mainwindow = 0;
ICore::ICore(Internal::MainWindow *mw)
{
    qDebug() << "ICore";
    m_mainwindow    = mw;
    m_instance      = this;
}

Core::ICore::~ICore()
{
    m_instance = 0;
    m_mainwindow = 0;
	Utils::XmlSerializableFactory::Release();
}

XmlSerializableFactory * Core::ICore::serializableFactory()
{
	return Utils::XmlSerializableFactory::instance();
}

ICore *ICore::instance()
{
    return m_instance;
}

QSettings *ICore::settings(QSettings::Scope scope)
{
    return m_mainwindow->settings(scope);
}

SettingsDatabase *ICore::settingsDatabase()
{
    return m_mainwindow->settingsDatabase();
}


QString ICore::resourcePath()
{
    QString str = QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String(SHARE_PATH));
    qDebug() << str;
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String(SHARE_PATH));
}

QString ICore::userResourcePath()
{
    const QString configDir = QFileInfo(settings(QSettings::UserScope)->fileName()).path();
    const QString urp = configDir + QLatin1String("/totem");

    QFileInfo fi(urp + QLatin1Char('/'));
    if (!fi.exists())
    {
        QDir dir;
        if (!dir.mkpath(urp))
            qWarning() << "could not create" << urp;
    }

    return urp;
}

QMainWindow *ICore::mainWindow()
{
    return m_mainwindow;
}

QStatusBar *ICore::statusBar()
{
    return m_mainwindow->statusBar();
}

IContext *ICore::currentContextObject()
{
    return m_mainwindow->currentContextObject();
}

void ICore::updateAdditionalContexts(const Context &remove, const Context &add)
{
    m_mainwindow->updateAdditionalContexts(remove, add);
}

bool ICore::hasContext(int context)
{
    return m_mainwindow->hasContext(context);
}

void ICore::addContextObject(IContext *context)
{
    m_mainwindow->addContextObject(context);
}

void ICore::removeContextObject(IContext *context)
{
    m_mainwindow->removeContextObject(context);
}

void ICore::addToolBar(QToolBar *toolBar)
{
    m_mainwindow->addModeTabToolBar(toolBar);
}

void ICore::removeToolBar(QToolBar *toolBar)
{
    m_mainwindow->removeModeTabToolBar(toolBar);
}

ActionManager *ICore::actionManager()
{
    return m_mainwindow->actionManager();
}

EditorManager *ICore::editorManager()
{
    return m_mainwindow->editorManager();
}

MessageManager *ICore::messageManager()
{
    return m_mainwindow->messageManager();
}

