#include "stdafx.h"
#include "mainwindow.h"
#include <QCloseEvent>
#include <QSettings>
#include "extensionsystem/pluginmanager.h"


static const char s_settingGroup[]		= "MainWindow";
static const char s_windowGeometryKey[] = "WindowGeometry";
static const char s_windowStateKey[]	= "WindowState";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_settings(NULL)
{
	m_settings = ExtensionSystem::PluginManager::instance()->settings();
}

MainWindow::~MainWindow()
{

}

void MainWindow::readSettings()
{
	if (m_settings)
	{
		m_settings->beginGroup(QLatin1String(s_settingGroup));
		if (restoreGeometry(m_settings->value(QLatin1String(s_windowGeometryKey)).toByteArray()))
			resize(1000, 700);
		restoreState(m_settings->value(QLatin1String(s_windowStateKey)).toByteArray());
		m_settings->endGroup();
	}
}

void MainWindow::writeSettings()
{
	if (m_settings)
	{
		m_settings->beginGroup(QLatin1String(s_settingGroup));
		m_settings->setValue(QLatin1String(s_windowGeometryKey), saveGeometry());
		m_settings->setValue(QLatin1String(s_windowStateKey), saveState());
		m_settings->endGroup();
	}
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	writeSettings();
	e->accept();
}
