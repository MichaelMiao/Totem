#include "stdafx.h"
#include "designnetcoreplugin.h"
#include "../../coreplugin/mainwindow/basefilewizard.h"
#include "designneteditorfactory.h"
#include "DesignNetManager.h"
#include "designnetsolutionwizard.h"


DesignNetCorePlugin::DesignNetCorePlugin()
{
}

DesignNetCorePlugin::~DesignNetCorePlugin()
{
}

bool DesignNetCorePlugin::initialize(const QStringList &arguments, QString *errorString)
{
	Core::BaseFileWizardParameters param;
	param.setIcon(QIcon(":/media/DesignNet.png"));
	param.setCategory(tr("DesignNet"));
	param.setDescription(tr("DesignNet about pattern recognition."));
	param.setDisplayCategory(tr("DesignNet File"));
	param.setDisplayName(tr("miao"));
	addAutoReleasedObject(new DesignNetSolutionWizard(param, this));
	addAutoReleasedObject(new DesignNetEditorFactory(this));
	DesignNetManager::instance()->init();

	return true;
}

void DesignNetCorePlugin::extensionsInitialized()
{

}
