#include "stdafx.h"
#include "basefilewizard.h"
#include <CustomUI/filewizarddialog.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/fileutils.h>
#include <utils/hostosinfo.h>
#include <utils/stringutils.h>
#include <utils/totemassert.h>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QIcon>
#include <QMessageBox>
#include <QScopedPointer>
#include <QSharedData>
#include <QSharedPointer>
#include <QVector>
#include <QWizard>
#include "../dialogs/promptoverwritedialog.h"
#include "../document/editormanager.h"
#include "../icore.h"
#include "ifilewizardextension.h"

enum { debugWizard = 0 };

namespace Core {

/**
 * \fn	static int indexOfFile(const GeneratedFiles &f, const QString &path)
 *
 * \brief	Searches for the first file.
 *
 *
 * \param	f   	The const GeneratedFiles &amp; to process.
 * \param	path	Full pathname of the file.
 *
 * \return	The zero-based index of the found file, or -1 if no match was found.
 */

static int indexOfFile(const GeneratedFiles &f, const QString &path)
{
    const int size = f.size();
    for (int i = 0; i < size; ++i)
        if (f.at(i).path() == path)
            return i;
    return -1;
}

// ------------ BaseFileWizardParameterData
class BaseFileWizardParameterData : public QSharedData
{
public:
    explicit BaseFileWizardParameterData(IWizard::WizardKind kind = IWizard::FileWizard);
    void clear();

    IWizard::WizardKind kind;
    QIcon icon;
    QString description;
    QString displayName;
    QString id;
    QString category;
    QString displayCategory;
    Core::IWizard::WizardFlags flags;
    QString descriptionImage;
};

BaseFileWizardParameterData::BaseFileWizardParameterData(IWizard::WizardKind k) :
    kind(k)
{
}

void BaseFileWizardParameterData::clear()
{
    kind = IWizard::FileWizard;
    icon = QIcon();
    description.clear();
    displayName.clear();
    id.clear();
    category.clear();
    displayCategory.clear();
}

/*!
    \class Core::BaseFileWizardParameters
    \brief 向导参数类，包括：种类（FileWizard), Icon, 描述，显示名称，id，分类，分类显示名
     

    \sa Core::GeneratedFile, Core::BaseFileWizard, Core::StandardFileWizard
    \sa Core::Internal::WizardEventLoop
*/

BaseFileWizardParameters::BaseFileWizardParameters(IWizard::WizardKind kind) :
   m_d(new BaseFileWizardParameterData(kind))
{
}

BaseFileWizardParameters::BaseFileWizardParameters(const BaseFileWizardParameters &rhs) :
    m_d(rhs.m_d)
{
}

BaseFileWizardParameters &BaseFileWizardParameters::operator=(const BaseFileWizardParameters &rhs)
{
    if (this != &rhs)
        m_d.operator=(rhs.m_d);
    return *this;
}

BaseFileWizardParameters::~BaseFileWizardParameters()
{
}

void BaseFileWizardParameters::clear()
{
    m_d->clear();
}

CORE_EXPORT QDebug operator<<(QDebug d, const BaseFileWizardParameters &p)
{
    d.nospace() << "Kind: " << p.kind() << " Id: " << p.id()
                << " Category: " << p.category()
                << " DisplayName: " << p.displayName()
                << " Description: " << p.description()
                << " DisplayCategory: " << p.displayCategory();
    return d;
}

IWizard::WizardKind BaseFileWizardParameters::kind() const
{
    return m_d->kind;
}

void BaseFileWizardParameters::setKind(IWizard::WizardKind k)
{
    m_d->kind = k;
}

QIcon BaseFileWizardParameters::icon() const
{
    return m_d->icon;
}

void BaseFileWizardParameters::setIcon(const QIcon &icon)
{
    m_d->icon = icon;
}

QString BaseFileWizardParameters::description() const
{
    return m_d->description;
}

void BaseFileWizardParameters::setDescription(const QString &v)
{
    m_d->description = v;
}

QString BaseFileWizardParameters::displayName() const
{
    return m_d->displayName;
}

void BaseFileWizardParameters::setDisplayName(const QString &v)
{
    m_d->displayName = v;
}

QString BaseFileWizardParameters::id() const
{
    return m_d->id;
}

void BaseFileWizardParameters::setId(const QString &v)
{
    m_d->id = v;
}

QString BaseFileWizardParameters::category() const
{
    return m_d->category;
}

void BaseFileWizardParameters::setCategory(const QString &v)
{
    m_d->category = v;
}

QString BaseFileWizardParameters::displayCategory() const
{
    return m_d->displayCategory;
}

/**
 * \fn	void BaseFileWizardParameters::setDisplayCategory(const QString &v)
 *
 * \brief	Sets display category.
 *
 * \author	Michael_BJFU
 * \date	2013/5/8
 *
 * \param	v	The const QString &amp; to process.
 */

void BaseFileWizardParameters::setDisplayCategory(const QString &v)
{
    m_d->displayCategory = v;
}

Core::IWizard::WizardFlags BaseFileWizardParameters::flags() const
{
    return m_d->flags;
}

void BaseFileWizardParameters::setFlags(Core::IWizard::WizardFlags flags)
{
    m_d->flags = flags;
}

QString BaseFileWizardParameters::descriptionImage() const
{
  return m_d->descriptionImage;
}

void BaseFileWizardParameters::setDescriptionImage(const QString &path)
{
    m_d->descriptionImage = path;
}

/*!
    \class Core::Internal::WizardEventLoop
    \brief Wizard的消息循环，为了不影响主消息循环

    Use by Core::BaseFileWizard to intercept the change from the standard wizard pages
    to the extension pages (as the latter require the list of Core::GeneratedFile generated).

	Synopsis:
	\code
	Wizard wizard(parent);
	WizardEventLoop::WizardResult wr;
	do {
	wr = WizardEventLoop::execWizardPage(wizard);
	} while (wr == WizardEventLoop::PageChanged);
	\endcode

    \sa Core::GeneratedFile, Core::BaseFileWizardParameters, Core::BaseFileWizard, Core::StandardFileWizard
*/

WizardEventLoop::WizardEventLoop(QObject *parent) :
    QEventLoop(parent),
    m_result(Rejected)
{
}

WizardEventLoop::WizardResult WizardEventLoop::execWizardPage(QWizard &wizard)
{
    WizardEventLoop *eventLoop = wizard.findChild<WizardEventLoop *>();
    if (!eventLoop)
	{
        eventLoop = new WizardEventLoop(&wizard);
        connect(&wizard, SIGNAL(currentIdChanged(int)), eventLoop, SLOT(pageChanged(int)));
        connect(&wizard, SIGNAL(accepted()), eventLoop, SLOT(accepted()));
        connect(&wizard, SIGNAL(rejected()), eventLoop, SLOT(rejected()));
        wizard.setAttribute(Qt::WA_ShowModal, true);
        wizard.show();
    }
    const WizardResult result = eventLoop->execWizardPageI();
    // Quitting?
    if (result != PageChanged)/// 停止向导
        delete eventLoop;
    if (debugWizard)
        qDebug() << "WizardEventLoop::runWizard" << wizard.pageIds() << " returns " << result;

    return result;
}

WizardEventLoop::WizardResult WizardEventLoop::execWizardPageI()
{
    m_result = Rejected;
    exec(QEventLoop::DialogExec);
    return m_result;
}

void WizardEventLoop::pageChanged(int /*page*/)
{
    m_result = PageChanged;
    quit(); // !
}

void WizardEventLoop::accepted()
{
    m_result = Accepted;
    quit();
}

void WizardEventLoop::rejected()
{
    m_result = Rejected;
    quit();
}

/*!
    \class Core::BaseFileWizard
    \brief 创建文件的通用类

    以下抽象方法必须实现：
    \list
    \li createWizardDialog(): 用来创建要显示的QWizard
    \li generateFiles(): 生成文件内容
    \endlist

    可以通过重写\c postGenerateFiles()，以便在生成文件之后做更多的操作。
    
	\sa Core::GeneratedFile, Core::BaseFileWizardParameters, Core::StandardFileWizard
    \sa Core::Internal::WizardEventLoop
*/

struct BaseFileWizardPrivate
{
    explicit BaseFileWizardPrivate(const Core::BaseFileWizardParameters &parameters)
      : m_parameters(parameters), m_wizardDialog(0)
    {}

    const Core::BaseFileWizardParameters m_parameters;
    QWizard *m_wizardDialog;
};

// ---------------- Wizard
BaseFileWizard::BaseFileWizard(const BaseFileWizardParameters &parameters,
                       QObject *parent) :
    IWizard(parent),
    d(new BaseFileWizardPrivate(parameters))
{
}

BaseFileWizardParameters BaseFileWizard::baseFileWizardParameters() const
{
    return d->m_parameters;
}

BaseFileWizard::~BaseFileWizard()
{
    delete d;
}

IWizard::WizardKind  BaseFileWizard::kind() const
{
    return d->m_parameters.kind();
}

QIcon BaseFileWizard::icon() const
{
    return d->m_parameters.icon();
}

QString BaseFileWizard::description() const
{
    return d->m_parameters.description();
}

QString BaseFileWizard::displayName() const
{
    return d->m_parameters.displayName();
}

QString BaseFileWizard::id() const
{
    return d->m_parameters.id();
}

QString BaseFileWizard::category() const
{
    return d->m_parameters.category();
}

QString BaseFileWizard::displayCategory() const
{
    return d->m_parameters.displayCategory();
}

QString BaseFileWizard::descriptionImage() const
{
       return d->m_parameters.descriptionImage();
}

void BaseFileWizard::runWizard(const QString &path, QWidget *parent, const QVariantMap &extraValues)
{
    TOTEM_ASSERT(!path.isEmpty(), return);

    typedef  QList<IFileWizardExtension*> ExtensionList;

    QString errorMessage;
    // Compile extension pages, purge out unused ones
    ExtensionList extensions = ExtensionSystem::PluginManager::instance()->getObjects<IFileWizardExtension>();
    WizardPageList  allExtensionPages;
    for (ExtensionList::iterator it = extensions.begin(); it !=  extensions.end(); ) {
        const WizardPageList extensionPages = (*it)->extensionPages(this);
        if (extensionPages.empty()) {
            it = extensions.erase(it);
        } else {
            allExtensionPages += extensionPages;
            ++it;
        }
    }

    if (debugWizard)
        qDebug() << Q_FUNC_INFO <<  path << parent << "exs" <<  extensions.size() << allExtensionPages.size();

    QWizardPage *firstExtensionPage = 0;
    if (!allExtensionPages.empty())
        firstExtensionPage = allExtensionPages.front();

    // Create dialog and run it. Ensure that the dialog is deleted when
    // leaving the func, but not before the IFileWizardExtension::process
    // has been called

    WizardDialogParameters::DialogParameterFlags dialogParameterFlags;

    if (flags().testFlag(ForceCapitalLetterForFileName))
        dialogParameterFlags |= WizardDialogParameters::ForceCapitalLetterForFileName;

    const QScopedPointer<QWizard> wizard(createWizardDialog(parent, WizardDialogParameters(path,
                                                                                           allExtensionPages,
                                                                                           dialogParameterFlags,
                                                                                           extraValues)));
    TOTEM_ASSERT(!wizard.isNull(), return);

    GeneratedFiles files;
    // Run the wizard: Call generate files on switching to the first extension
    // page is OR after 'Accepted' if there are no extension pages
    while (true) {
        const WizardEventLoop::WizardResult wr = WizardEventLoop::execWizardPage(*wizard);
        if (wr == WizardEventLoop::Rejected) {
            files.clear();
            break;
        }
        const bool accepted = wr == WizardEventLoop::Accepted;
        const bool firstExtensionPageHit = wr == WizardEventLoop::PageChanged
                                           && wizard->page(wizard->currentId()) == firstExtensionPage;
        const bool needGenerateFiles = firstExtensionPageHit || (accepted && allExtensionPages.empty());
        if (needGenerateFiles) {
            QString errorMessage;
            files = generateFiles(wizard.data(), &errorMessage);
            if (files.empty()) {
                QMessageBox::critical(0, tr("File Generation Failure"), errorMessage);
                break;
            }
        }
        if (firstExtensionPageHit)
            foreach (IFileWizardExtension *ex, extensions)
                ex->firstExtensionPageShown(files, extraValues);
        if (accepted)
            break;
    }
    if (files.empty())
        return;
    // Compile result list and prompt for overwrite
    switch (promptOverwrite(&files, &errorMessage)) {
    case OverwriteCanceled:
        return;
    case OverwriteError:
        QMessageBox::critical(0, tr("Existing files"), errorMessage);
        return;
    case OverwriteOk:
        break;
    }

    foreach (IFileWizardExtension *ex, extensions) {
        for (int i = 0; i < files.count(); i++) {
            ex->applyCodeStyle(&files[i]);
        }
    }

    // Write
    if (!writeFiles(files, &errorMessage)) {
        QMessageBox::critical(parent, tr("File Generation Failure"), errorMessage);
        return;
    }

    bool removeOpenProjectAttribute = false;
    // Run the extensions
    foreach (IFileWizardExtension *ex, extensions) {
        bool remove;
        if (!ex->processFiles(files, &remove, &errorMessage)) {
            QMessageBox::critical(parent, tr("File Generation Failure"), errorMessage);
            return;
        }
        removeOpenProjectAttribute |= remove;
    }

    if (removeOpenProjectAttribute) {
        for (int i = 0; i < files.count(); i++) {
            if (files[i].attributes() & Core::GeneratedFile::OpenProjectAttribute)
                files[i].setAttributes(Core::GeneratedFile::OpenEditorAttribute);
        }
    }

    // Post generation handler
    if (!postGenerateFiles(wizard.data(), files, &errorMessage))
        if (!errorMessage.isEmpty())
            QMessageBox::critical(0, tr("File Generation Failure"), errorMessage);
}


Core::IWizard::WizardFlags BaseFileWizard::flags() const
{
    return d->m_parameters.flags();
}

/*!
    \fn virtual QWizard *Core::BaseFileWizard::createWizardDialog(QWidget *parent,
                                                                  const QString &defaultPath,
                                                                  const WizardPageList &extensionPages) const = 0
    \brief Implement to create the wizard dialog on the parent, adding the extension pages.
*/

/*!
    \fn virtual Core::GeneratedFiles Core::BaseFileWizard::generateFiles(const QWizard *w,
                                                                         QString *errorMessage) const = 0
     \brief Overwrite to query the parameters from the dialog and generate the files.

     Note: This does not generate physical files, but merely the list of Core::GeneratedFile.
*/

/*!
    \brief Physically write files.

    Re-implement (calling the base implementation) to create files with CustomGeneratorAttribute set.
*/

bool BaseFileWizard::writeFiles(const GeneratedFiles &files, QString *errorMessage)
{
    const GeneratedFile::Attributes noWriteAttributes
        = GeneratedFile::CustomGeneratorAttribute|GeneratedFile::KeepExistingFileAttribute;
    foreach (const GeneratedFile &generatedFile, files)
        if (!(generatedFile.attributes() & noWriteAttributes ))
            if (!generatedFile.write(errorMessage))
                return false;
    return true;
}

/*!
    \brief 设置QWizard的选项.
	QWizard::NoCancelButton | QWizard::NoDefaultButton | QWizard::NoBackButtonOnStartPage  --- & ~Qt::WindowContextHelpButtonHint
*/

void BaseFileWizard::setupWizard(QWizard *w)
{
    w->setOption(QWizard::NoCancelButton, false);
    w->setOption(QWizard::NoDefaultButton, false);
    w->setOption(QWizard::NoBackButtonOnStartPage, true);
    w->setWindowFlags(w->windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

/*!
    \brief 可以重写，该函数在生成文件后调用

    默认实现是打开新产生的文件
*/

bool BaseFileWizard::postGenerateFiles(const QWizard *, const GeneratedFiles &l, QString *errorMessage)
{
    return BaseFileWizard::postGenerateOpenEditors(l, errorMessage);
}

/*!
    \brief 打开指定文件
*/

bool BaseFileWizard::postGenerateOpenEditors(const GeneratedFiles &l, QString *errorMessage)
{
    foreach (const Core::GeneratedFile &file, l) {
        if (file.attributes() & Core::GeneratedFile::OpenEditorAttribute) {
            if (!Core::EditorManager::openEditor(file.path(), file.editorId() )) {
                if (errorMessage)
                    *errorMessage = tr("Failed to open an editor for '%1'.").arg(QDir::toNativeSeparators(file.path()));
                return false;
            }
        }
    }
    return true;
}

/*!
    \brief Utility that performs an overwrite check on a set of files. It checks if
    the file exists, can be overwritten at all and prompts the user with a summary.
*/

BaseFileWizard::OverwriteResult BaseFileWizard::promptOverwrite(GeneratedFiles *files,
                                                                QString *errorMessage) const
{
    if (debugWizard)
        qDebug() << Q_FUNC_INFO << files;

    QStringList existingFiles;
    bool oddStuffFound = false;

    static const QString readOnlyMsg = tr(" [read only]");
    static const QString directoryMsg = tr(" [folder]");
    static const QString symLinkMsg = tr(" [symbolic link]");

    foreach (const GeneratedFile &file, *files) {
        const QFileInfo fi(file.path());
        if (fi.exists())
            existingFiles.append(file.path());
    }
    if (existingFiles.isEmpty())
        return OverwriteOk;
    // Before prompting to overwrite existing files, loop over files and check
    // if there is anything blocking overwriting them (like them being links or folders).
    // Format a file list message as ( "<file1> [readonly], <file2> [folder]").
    const QString commonExistingPath = Utils::commonPath(existingFiles);
    QString fileNamesMsgPart;
    foreach (const QString &fileName, existingFiles) {
        const QFileInfo fi(fileName);
        if (fi.exists()) {
            if (!fileNamesMsgPart.isEmpty())
                fileNamesMsgPart += QLatin1String(", ");
            fileNamesMsgPart += QDir::toNativeSeparators(fileName.mid(commonExistingPath.size() + 1));
            do {
                if (fi.isDir()) {
                    oddStuffFound = true;
                    fileNamesMsgPart += directoryMsg;
                    break;
                }
                if (fi.isSymLink()) {
                    oddStuffFound = true;
                    fileNamesMsgPart += symLinkMsg;
                    break;
				}
                if (!fi.isWritable()) {
                    oddStuffFound = true;
                    fileNamesMsgPart += readOnlyMsg;
                }
            } while (false);
        }
    }

    if (oddStuffFound) {
        *errorMessage = tr("The project directory %1 contains files which cannot be overwritten:\n%2.")
                .arg(QDir::toNativeSeparators(commonExistingPath)).arg(fileNamesMsgPart);
        return OverwriteError;
    }
    // Prompt to overwrite existing files.
    Internal::PromptOverwriteDialog overwriteDialog;
    // Scripts cannot handle overwrite
    overwriteDialog.setFiles(existingFiles);
    foreach (const GeneratedFile &file, *files)
        if (file.attributes() & GeneratedFile::CustomGeneratorAttribute)
            overwriteDialog.setFileEnabled(file.path(), false);
    if (overwriteDialog.exec() != QDialog::Accepted)
        return OverwriteCanceled;
    const QStringList existingFilesToKeep = overwriteDialog.uncheckedFiles();
    if (existingFilesToKeep.size() == files->size()) // All exist & all unchecked->Cancel.
        return OverwriteCanceled;
    // Set 'keep' attribute in files
    foreach (const QString &keepFile, existingFilesToKeep) {
        const int i = indexOfFile(*files, keepFile);
        TOTEM_ASSERT(i != -1, return OverwriteCanceled);
        GeneratedFile &file = (*files)[i];
        file.setAttributes(file.attributes() | GeneratedFile::KeepExistingFileAttribute);
    }
    return OverwriteOk;
}

/*!
    \brief 自动补充文件后缀名，若已经存在，就直接返回了
*/

QString BaseFileWizard::buildFileName(const QString &path,
                                      const QString &baseName,
                                      const QString &extension)
{
    QString rc = path;
    if (!rc.isEmpty() && !rc.endsWith(QDir::separator()))
        rc += QDir::separator();
    rc += baseName;
    // Add extension unless user specified something else
    const QChar dot = QLatin1Char('.');
    if (!extension.isEmpty() && !baseName.contains(dot)) {
        if (!extension.startsWith(dot))
            rc += dot;
        rc += extension;
    }
    if (debugWizard)
        qDebug() << Q_FUNC_INFO << rc;
    return rc;
}

void BaseFileWizard::applyExtensionPageShortTitle( CustomUI::Wizard *wizard, int pageId )
{
	if (pageId < 0)
		return;
	QWizardPage *p = wizard->page(pageId);
	if (!p)
		return;
	CustomUI::WizardProgressItem *item = wizard->wizardProgress()->item(pageId);
	if (!item)
		return;
	const QString shortTitle = p->property("shortTitle").toString();
	if (!shortTitle.isEmpty())
		item->setTitle(shortTitle);
}

/*!
    \class Core::StandardFileWizard
    \brief 创建文件的辅助类

    It uses Utils::FileWizardDialog and introduces a new virtual to generate the
    files from path and name.

    \sa Core::GeneratedFile, Core::BaseFileWizardParameters, Core::BaseFileWizard
    \sa Core::Internal::WizardEventLoop
*/

/*!
    \fn Core::GeneratedFiles Core::StandardFileWizard::generateFilesFromPath(const QString &path,
                                                                             const QString &name,
                                                                             QString *errorMessage) const = 0
    \brief Newly introduced virtual that creates the files under the path.
*/

StandardFileWizard::StandardFileWizard(const BaseFileWizardParameters &parameters,
                                       QObject *parent) :
    BaseFileWizard(parameters, parent)
{
}

/*!
    \brief 用来创建 CustomUI::FileWizardDialog.
*/

QWizard *StandardFileWizard::createWizardDialog(QWidget *parent,
                                                const WizardDialogParameters &wizardDialogParameters) const
{
    CustomUI::FileWizardDialog *standardWizardDialog = new CustomUI::FileWizardDialog(parent);
    if (wizardDialogParameters.flags().testFlag(WizardDialogParameters::ForceCapitalLetterForFileName))
        standardWizardDialog->setForceFirstCapitalLetterForFileName(true);
    standardWizardDialog->setWindowTitle(tr("New %1").arg(displayName()));
    setupWizard(standardWizardDialog);
    standardWizardDialog->setPath(wizardDialogParameters.defaultPath());
    foreach (QWizardPage *p, wizardDialogParameters.extensionPages())
        BaseFileWizard::applyExtensionPageShortTitle(standardWizardDialog, standardWizardDialog->addPage(p));
    return standardWizardDialog;
}

/*!
    \brief 生成文件，调用generateFilesFromPath(path, name, errormessage)
*/

GeneratedFiles StandardFileWizard::generateFiles(const QWizard *w,
                                                 QString *errorMessage) const
{
    const CustomUI::FileWizardDialog *standardWizardDialog = qobject_cast<const CustomUI::FileWizardDialog *>(w);
    return generateFilesFromPath(standardWizardDialog->path(),
                                 standardWizardDialog->fileName(),
                                 errorMessage);
}

} // namespace Core
