#pragma once

#include "../../coreplugin/generatedfile.h"
#include "../../coreplugin/mainwindow/basefilewizard.h"


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
