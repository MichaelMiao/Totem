#include "designnetsolutionwizard.h"
#include "CustomUI/wizard.h"
#include "CustomUI/filewizarddialog.h"
#include <QIcon>
using namespace Core;
using namespace CustomUI;
namespace DesignNet{
DesignNetSolutionWizard::DesignNetSolutionWizard(Core::BaseFileWizardParameters &p, QObject *parent)
	: Core::BaseFileWizard(p)
{

}

DesignNetSolutionWizard::~DesignNetSolutionWizard()
{

}

/**
 * \fn	QWizard * DesignNet::DesignNetSolutionWizard::createWizardDialog(QWidget *parent,
 * 		const WizardDialogParameters &wizardDialogParameters) const
 *
 * \brief	Creates wizard dialog.
 *
 * \author	Michael_BJFU
 * \date	2013/5/9
 *
 * \param [in,out]	parent		  	If non-null, the parent.
 * \param	wizardDialogParameters	Options for controlling the wizard dialog.
 *
 * \return	null if it fails, else the new wizard dialog.
 */

QWizard * DesignNet::DesignNetSolutionWizard::createWizardDialog(QWidget *parent, const WizardDialogParameters &wizardDialogParameters) const
{
	CustomUI::FileWizardDialog *wizard = new CustomUI::FileWizardDialog(parent);
	return(wizard);
}

/**
 * \fn	GeneratedFiles DesignNet::DesignNetSolutionWizard::generateFiles(const QWizard *w,
 * 		QString *errorMessage) const
 *
 * \brief	Generates the files.
 *
 * \author	Michael_BJFU
 * \date	2013/5/9
 *
 * \param	w						The const QWizard * to process.
 * \param [in,out]	errorMessage	If non-null, message describing the error.
 *
 * \return	The files.
 */

GeneratedFiles DesignNet::DesignNetSolutionWizard::generateFiles(const QWizard *w, QString *errorMessage) const
{
	const FileWizardDialog *wizard = qobject_cast< const FileWizardDialog *>(w);
	const QString projectPath = wizard->path();
	const QString profileName = Core::BaseFileWizard::buildFileName(projectPath, wizard->fileName(), "txt");

	Core::GeneratedFile profile(profileName);
	profile.setAttributes(Core::GeneratedFile::OpenProjectAttribute | Core::GeneratedFile::OpenEditorAttribute);
	return Core::GeneratedFiles() << profile;
}

Core::BaseFileWizardParameters DesignNet::DesignNetSolutionWizard::parameters()
{
	Core::BaseFileWizardParameters param;
	param.setIcon(QIcon(":/media/DesignNet.png"));
	param.setCategory(tr("DesignNet"));
	param.setDescription(tr(""));
	param.setDisplayCategory(tr(""));
	param.setDisplayName(tr("miao"));
	return param;
}
}


