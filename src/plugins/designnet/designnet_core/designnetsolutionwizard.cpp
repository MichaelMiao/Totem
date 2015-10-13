#include "stdafx.h"
#include "designnetsolutionwizard.h"
#include <QIcon>
#include "CustomUI/filewizarddialog.h"


using namespace Core;
using namespace CustomUI;
DesignNetSolutionWizard::DesignNetSolutionWizard(Core::BaseFileWizardParameters &p, QObject *parent)
	: Core::BaseFileWizard(p)
{

}

DesignNetSolutionWizard::~DesignNetSolutionWizard()
{

}

QWizard * DesignNetSolutionWizard::createWizardDialog(QWidget *parent, const Core::WizardDialogParameters &wizardDialogParameters) const
{
	CustomUI::FileWizardDialog *wizard = new CustomUI::FileWizardDialog(parent);
	return(wizard);
}

Core::BaseFileWizardParameters DesignNetSolutionWizard::parameters()
{
	Core::BaseFileWizardParameters param;
	param.setIcon(QIcon(":/media/DesignNet.png"));
	param.setCategory(tr("DesignNet"));
	param.setDescription(tr("The file is used for flowers category recognition"));
	param.setDisplayCategory(tr("DesignNet File"));
	param.setDisplayName(tr("miao"));
	return param;
}

GeneratedFiles DesignNetSolutionWizard::generateFiles(const QWizard *w, QString *errorMessage) const
{
	const FileWizardDialog *wizard = qobject_cast< const FileWizardDialog *>(w);
	const QString projectPath = wizard->path();
	const QString profileName = Core::BaseFileWizard::buildFileName(projectPath, wizard->fileName(), "txt");

	Core::GeneratedFile profile(profileName);
	profile.setAttributes(Core::GeneratedFile::OpenProjectAttribute | Core::GeneratedFile::OpenEditorAttribute);
	return Core::GeneratedFiles() << profile;
}
