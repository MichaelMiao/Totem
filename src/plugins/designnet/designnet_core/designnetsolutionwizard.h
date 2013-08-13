#ifndef DESIGNNETSOLUTIONWIZARD_H
#define DESIGNNETSOLUTIONWIZARD_H

#include "coreplugin/basefilewizard.h"
#include "coreplugin/generatedfile.h"

namespace DesignNet{
class DesignNetSolutionWizard : public Core::BaseFileWizard
{
	Q_OBJECT

public:
	DesignNetSolutionWizard(Core::BaseFileWizardParameters &p, QObject *parent);
	~DesignNetSolutionWizard();
	static Core::BaseFileWizardParameters parameters();
	
protected:
	virtual QWizard *createWizardDialog(QWidget *parent,
		const Core::WizardDialogParameters &wizardDialogParameters) const;

	virtual Core::GeneratedFiles generateFiles(const QWizard *w,
		QString *errorMessage) const;
	
};
}

#endif // DESIGNNETSOLUTIONWIZARD_H
