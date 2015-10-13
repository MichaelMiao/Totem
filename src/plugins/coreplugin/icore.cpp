#include "stdafx.h"
#include "icore.h"
#include <QApplication>
#include "actionmanager/actionmanager.h"
#include "extensionsystem/pluginmanager.h"
#include "mainwindow/mainwindow.h"


//../share/totem 存放公共配置文件
#define SHARE_PATH "/../share"

Core::SettingsDatabase* ICore::m_pSettingsDatabase = NULL;

void ICore::initialize()
{
	Core::ActionManager::instance();
	connect(this, SIGNAL(quit()), qApp, SLOT(quit()));
	MainWindow* pMain = createMainWindow();
	pMain->show();
}

void ICore::extensionsInitialized()
{

}

MainWindow* ICore::createMainWindow()
{
	MainWindow* pNewWindow = new MainWindow();
	if (!m_pFirstMainWindow)
		m_pFirstMainWindow = pNewWindow;
	
	pNewWindow->init();
	connect(pNewWindow, SIGNAL(closed()), this, SLOT(onMainWindowClosed()));
	return pNewWindow;
}

void ICore::onMainWindowClosed()
{
	MainWindow* mainwindow = qobject_cast<MainWindow*>(sender());
	if (mainwindow)
		emit quit();
}

void ICore::insertCenterWidget(QWidget* pWidget)
{
	m_pFirstMainWindow->insertCenterWidget(pWidget);
}

QString ICore::resourcePath()
{
	QString str = QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String(SHARE_PATH));
	qDebug() << str;
	return QDir::cleanPath(QCoreApplication::applicationDirPath() + QLatin1String(SHARE_PATH));
}

QString ICore::userResourcePath()
{
	QSettings* pSettings = ExtensionSystem::PluginManager::instance()->settings();
	const QString configDir = QFileInfo(pSettings->fileName()).path();
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

void ICore::addContextObject(Core::IContext *context)
{
	if (m_pFirstMainWindow)
		m_pFirstMainWindow->addContextObject(context);
}

void ICore::removeContextObject(Core::IContext *context)
{
	if (m_pFirstMainWindow)
		m_pFirstMainWindow->removeContextObject(context);
}

QSettings * ICore::settings(QSettings::Scope scope /*= QSettings::UserScope*/)
{
	if (scope == QSettings::UserScope)
		return ExtensionSystem::PluginManager::instance()->settings();
	else
		return ExtensionSystem::PluginManager::instance()->globalSettings();
}

void ICore::saveSettings()
{
	ICore::settings(QSettings::SystemScope)->sync();
	ICore::settings(QSettings::UserScope)->sync();
}

ICore::ICore() : m_pFirstMainWindow(0)
{
	m_pSettingsDatabase = new Core::SettingsDatabase(QFileInfo(settings()->fileName()).path(),
                                                                      QLatin1String("Totem"),
                                                                      this);
}
