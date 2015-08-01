#pragma once
#include "../../libs/extensionsystem/iplugin.h"

class AnimationEditorPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.michaelmiao.totem.AnimationEditorPlugin")
	Q_INTERFACES(ExtensionSystem::IPlugin)

public:

	AnimationEditorPlugin();
	~AnimationEditorPlugin(){}

	bool initialize(const QStringList &arguments, QString *errorString) override;
	void extensionsInitialized() override;

private:


};