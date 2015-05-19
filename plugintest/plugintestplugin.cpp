#include "plugintest.h"

#include <QtCore/QtPlugin>
#include <QIcon>
#include "plugintestplugin.h"


plugintestPlugin::plugintestPlugin(QObject *parent)
{
	initialized = false;
}

bool plugintestPlugin::isInitialized() const
{
	return initialized;
}

QWidget *plugintestPlugin::createWidget(QWidget *parent)
{
	return new plugintest(parent);
}

QString plugintestPlugin::name() const
{
	return "plugintest";
}

QString plugintestPlugin::group() const
{
	return "My Plugins";
}

QIcon plugintestPlugin::icon() const
{
	return QIcon();
}

QString plugintestPlugin::toolTip() const
{
	return QString();
}

QString plugintestPlugin::whatsThis() const
{
	return QString();
}

bool plugintestPlugin::isContainer() const
{
	return false;
}

QString plugintestPlugin::domXml() const
{
	return "<widget class=\"plugintest\" name=\"plugintest\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>100</width>\n"
		"   <height>100</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QString plugintestPlugin::includeFile() const
{
	return "plugintest.h";
}

