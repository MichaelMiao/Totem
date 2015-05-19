#ifndef PLUGINTESTPLUGIN_H
#define PLUGINTESTPLUGIN_H

#include "../src/libs/extensionsystem/iplugin.h"

class plugintestPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.michaelmiao.totem.PluginInterface")
	Q_INTERFACES(ExtensionSystem::IPlugin)

public:
	plugintestPlugin(QObject *parent = 0);

	bool isContainer() const;
	bool isInitialized() const;
	QIcon icon() const;
	QString domXml() const;
	QString group() const;
	QString includeFile() const;
	QString name() const;
	QString toolTip() const;
	QString whatsThis() const;
	QWidget *createWidget(QWidget *parent);

	virtual bool initialize(const QStringList &arguments, QString *errorString) { return true; }
	virtual void extensionsInitialized() {}

private:
	bool initialized;
};

#endif // PLUGINTESTPLUGIN_H
