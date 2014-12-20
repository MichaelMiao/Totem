#include "AnimationEditorPlugin.h"
#include "coreplugin/coreconstants.h"
#include "coreplugin/modemanager.h"
#include "AnimationEditMode.h"

using namespace Core;
AnimationEditorPlugin::AnimationEditorPlugin()
{

}

bool AnimationEditorPlugin::initialize(const QStringList &arguments, QString *errorString)
{
	AnimationEditMode* pMode = new AnimationEditMode;
	addAutoReleasedObject(pMode);
	ModeManager::activateMode(pMode->id());
	return true;
}

void AnimationEditorPlugin::extensionsInitialized()
{
}
