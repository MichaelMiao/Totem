#pragma once
#include <QEventLoop>
#include <QVariantMap>
#include "../core_global.h"
#include "../generatedfile.h"
#include "../iwizard.h"
#include "extensionsystem/iplugin.h"


QT_BEGIN_NAMESPACE
class QWizard;
class QWizardPage;
QT_END_NAMESPACE
namespace CustomUI{ class Wizard; }
namespace Core{
class BaseFileWizardParameterData;
class CORE_EXPORT BaseFileWizardParameters
{
public:
	explicit BaseFileWizardParameters(IWizard::WizardKind kind = IWizard::FileWizard);
	BaseFileWizardParameters(const BaseFileWizardParameters &);
	BaseFileWizardParameters &operator=(const BaseFileWizardParameters&);
	~BaseFileWizardParameters();

	void clear();

	IWizard::WizardKind kind() const;
	void setKind(IWizard::WizardKind k);

	QIcon icon() const;
	void setIcon(const QIcon &icon);

	QString description() const;
	void setDescription(const QString &description);

	QString displayName() const;
	void setDisplayName(const QString &name);

	QString id() const;
	void setId(const QString &id);

	QString category() const;
	void setCategory(const QString &category);

	QString displayCategory() const;
	void setDisplayCategory(const QString &trCategory);

	QString descriptionImage() const;
	void setDescriptionImage(const QString &path);

	Core::IWizard::WizardFlags flags() const;
	void setFlags(Core::IWizard::WizardFlags flags);
private:
	QSharedDataPointer<BaseFileWizardParameterData> m_d;
};

class CORE_EXPORT WizardDialogParameters
{
public:
	typedef QList<QWizardPage *> WizardPageList;

	enum DialogParameterEnum {
		ForceCapitalLetterForFileName = 0x01
	};
	Q_DECLARE_FLAGS(DialogParameterFlags, DialogParameterEnum)

	explicit WizardDialogParameters(const QString &defaultPath, const WizardPageList &extensionPages,
		DialogParameterFlags flags,
		QVariantMap extraValues)
		: m_defaultPath(defaultPath),
		m_extensionPages(extensionPages),
		m_parameterFlags(flags),
		m_extraValues(extraValues)
	{}

	QString defaultPath() const
	{ return m_defaultPath; }

	WizardPageList extensionPages() const
	{ return m_extensionPages; }

	DialogParameterFlags flags() const
	{ return m_parameterFlags; }

	QVariantMap extraValues() const
	{ return m_extraValues; }

private:
	QString m_defaultPath;
	WizardPageList m_extensionPages;
	DialogParameterFlags m_parameterFlags;
	QVariantMap m_extraValues;
};

/**
 * \class	BaseFileWizard
 *
 * \brief	BaseFile文件向导，提供了IWizard的基本实现
 *
 * \author	Michael_BJFU
 * \date	2013/5/8
 */
class BaseFileWizardPrivate;
class WizardEventLoop : public QEventLoop
{
	Q_OBJECT
		WizardEventLoop(QObject *parent);

public:
	enum WizardResult { Accepted, Rejected, PageChanged };

	static WizardResult execWizardPage(QWizard &w);

	private slots:
	void pageChanged(int);
	void accepted();
	void rejected();

private:
	WizardResult execWizardPageI();

	WizardResult m_result;
};

class CORE_EXPORT BaseFileWizard : public IWizard
{
	Q_OBJECT

public:

	virtual ~BaseFileWizard();

	// IWizard
	virtual WizardKind kind() const;
	virtual QIcon icon() const;
	virtual QString description() const;
	virtual QString displayName() const;
	virtual QString id() const;

	virtual QString category() const;
	virtual QString displayCategory() const;

	virtual QString descriptionImage() const;

	virtual void runWizard(const QString &path, QWidget *parent, const QVariantMap &extraValues);
	virtual WizardFlags flags() const;

	static QString buildFileName(const QString &path, const QString &baseName, const QString &extension);
	static void setupWizard(QWizard *);

protected:
	typedef QList<QWizardPage *> WizardPageList;

	explicit BaseFileWizard(const BaseFileWizardParameters &parameters, QObject *parent = 0);

	BaseFileWizardParameters baseFileWizardParameters() const;

	virtual QWizard *createWizardDialog(QWidget *parent,
		const WizardDialogParameters &wizardDialogParameters) const = 0;

	virtual GeneratedFiles generateFiles(const QWizard *w,
		QString *errorMessage) const = 0;

	virtual bool writeFiles(const GeneratedFiles &files, QString *errorMessage);

	virtual bool postGenerateFiles(const QWizard *w, const GeneratedFiles &l, QString *errorMessage);

	enum OverwriteResult { OverwriteOk,  OverwriteError,  OverwriteCanceled };
	OverwriteResult promptOverwrite(GeneratedFiles *files,
		QString *errorMessage) const;
	static bool postGenerateOpenEditors(const GeneratedFiles &l, QString *errorMessage = 0);
	static void applyExtensionPageShortTitle(CustomUI::Wizard *wizard, int pageId);

private:
	BaseFileWizardPrivate *d;
};

class CORE_EXPORT StandardFileWizard : public BaseFileWizard
{
	Q_OBJECT

protected:
	explicit StandardFileWizard(const BaseFileWizardParameters &parameters, QObject *parent = 0);
	virtual QWizard *createWizardDialog(QWidget *parent, const WizardDialogParameters &wizardDialogParameters) const;
	virtual GeneratedFiles generateFiles(const QWizard *w, QString *errorMessage) const;
	virtual GeneratedFiles generateFilesFromPath(const QString &path, const QString &name,
		QString *errorMessage) const = 0;
};

template <class WizardClass>
QList<WizardClass*> createMultipleBaseFileWizardInstances(const QList<BaseFileWizardParameters> &parametersList, ExtensionSystem::IPlugin *plugin)
{
	QList<WizardClass*> list;
	foreach (const BaseFileWizardParameters &parameters, parametersList) {
		WizardClass *wc = new WizardClass(parameters, 0);
		plugin->addAutoReleasedObject(wc);
		list << wc;
	}
	return list;
}
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Core::GeneratedFile::Attributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(Core::WizardDialogParameters::DialogParameterFlags)
