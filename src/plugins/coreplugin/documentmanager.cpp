#include "documentmanager.h"
#include "coreconstants.h"
#include "icore.h"
#include "ieditorfactory.h"
#include "idocument.h"
#include "ieditor.h"
#include "editormanager.h"
#include "utils/reloadpromptutils.h"
#include "utils/totemassert.h"
#include "dialogs/saveitemsdialog.h"
#include "id.h"

#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QFileSystemWatcher>
#include <QSet>
#include <QApplication>
#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>


static const char settingsGroupC[] = "RecentFiles";
static const char filesKeyC[] = "Files";
static const char editorsKeyC[] = "EditorIds";

//static const char directoryGroupC[] = "Directories";

namespace Core{

namespace Internal{
struct OpenWithEntry
{
    OpenWithEntry(){}
    IEditorFactory *editorFactory;
    QString fileName;
};

struct FileStateItem
{
    QDateTime modifiedTime;
    QFile::Permissions permissions;
};

struct FileState
{
    QMap<IDocument *, FileStateItem> lastUpdatedStates;
    FileStateItem expected;
};

struct DocumentManagerPrivate
{
    explicit DocumentManagerPrivate(QMainWindow *mw);
    QFileSystemWatcher *fileWatcher();

    QMap<QString, FileState> m_states;
    QSet<QString> m_changedFiles;
    QList<IDocument *> m_documentsWithoutWatch;
    QMap<IDocument *, QStringList> m_documentsWithWatch;
    QSet<QString> m_expectedFileNames;

    QList<DocumentManager::RecentFile> m_recentFiles;

    static const int m_maxRecentFiles = 7;

    QString m_currentFile;

    QMainWindow *m_mainWindow;
    QFileSystemWatcher *m_fileWatcher;

    bool m_blockActivated;
    QString m_lastVisitedDirectory;

    IDocument *m_blockedIDocument;


};
static DocumentManager *m_instance = 0;
static Internal::DocumentManagerPrivate *d = 0;
static QList<IDocument *> saveModifiedFilesHelper(const QList<IDocument *> &documents,
                               bool *cancelled, bool silently,
                               const QString &message,
                               const QString &alwaysSaveMessage = QString(),
                               bool *alwaysSave = 0);


static void removeFileInfo(IDocument *document)
{
    if(!d->m_documentsWithWatch.contains(document))
        return;
    foreach(const QString &fileName, d->m_documentsWithWatch.value(document))
    {
        if(!d->m_states.contains(fileName))
            continue;
        d->m_states[fileName].lastUpdatedStates.remove(document);
        if (d->m_states.value(fileName).lastUpdatedStates.isEmpty())
        {
            if (d->m_fileWatcher && d->m_fileWatcher->files().contains(fileName))
                d->m_fileWatcher->removePath(fileName);
            d->m_states.remove(fileName);
        }
    }
    d->m_documentsWithWatch.remove(document);
}
static void addFileInfo(const QString &fileName, IDocument *document)
{
    FileStateItem state;
    if (!fileName.isEmpty())
    {
        const QFileInfo fi(fileName);
        state.modifiedTime = fi.lastModified();
        state.permissions = fi.permissions();

        if (!d->m_states.contains(fileName))
        {
            d->m_states.insert(fileName, FileState());
        }
        QFileSystemWatcher *watcher = 0;
        watcher = d->fileWatcher();
        if (!watcher->files().contains(fileName))
            watcher->addPath(fileName);

        d->m_states[fileName].lastUpdatedStates.insert(document, state);
    }
    d->m_documentsWithWatch[document].append(fileName);
}
static void addFileInfo(IDocument *document)
{
    const QString fixedName = DocumentManager::fixFileName(document->fileName(), DocumentManager::KeepLinks);
    const QString fixedResolvedName = DocumentManager::fixFileName(document->fileName(), DocumentManager::ResolveLinks);
    addFileInfo(fixedResolvedName, document);
    if (fixedName != fixedResolvedName)
        addFileInfo(fixedName, document);
}

static QList<IDocument *> saveModifiedFilesHelper(const QList<IDocument *> &documents,
                                              bool *cancelled,
                                              bool silently,
                                              const QString &message,
                                              const QString &alwaysSaveMessage,
                                              bool *alwaysSave)
{
    if (cancelled)
        (*cancelled) = false;

    QList<IDocument *> notSaved;
    QMap<IDocument *, QString> modifiedDocumentsMap;
    QList<IDocument *> modifiedDocuments;

    foreach (IDocument *document, documents)
    {
        if (document->isModified())
        {
            QString name = document->fileName();
            if (name.isEmpty())
                name = document->suggestedFileName();

            // There can be several IDocuments pointing to the same file
            // Prefer one that is not readonly
            // (even though it *should* not happen that the IDocuments are inconsistent with readonly)
            if (!modifiedDocumentsMap.key(name, 0) || !document->isFileReadOnly())
                modifiedDocumentsMap.insert(document, name);
        }
    }
    modifiedDocuments = modifiedDocumentsMap.keys();
    if (!modifiedDocuments.isEmpty())
    {
        QList<IDocument *> documentsToSave;
        if (silently)
        {
            documentsToSave = modifiedDocuments;
        }
        else
        {
            SaveItemsDialog dia(d->m_mainWindow, modifiedDocuments);
            if (!message.isEmpty())
                dia.setMessage(message);
            if (!alwaysSaveMessage.isNull())
                dia.setAlwaysSaveMessage(alwaysSaveMessage);
            if (dia.exec() != QDialog::Accepted)
            {
                if (cancelled)
                    (*cancelled) = true;
                if (alwaysSave)
                    *alwaysSave = dia.alwaysSaveChecked();
                notSaved = modifiedDocuments;
                return notSaved;
            }
            if (alwaysSave)
                *alwaysSave = dia.alwaysSaveChecked();
            documentsToSave = dia.itemsToSave();
        }

        foreach (IDocument *document, documentsToSave)
        {
            if (!EditorManager::instance()->saveDocument(document))
            {
                if (cancelled)
                    *cancelled = true;
                notSaved.append(document);
            }
        }
    }
    return notSaved;
}
DocumentManagerPrivate::DocumentManagerPrivate(QMainWindow *mw)
    : m_mainWindow(mw),
      m_fileWatcher(0),
      m_blockActivated(false),
      m_lastVisitedDirectory(QDir::currentPath()),
      m_blockedIDocument(0)
{
}

QFileSystemWatcher *DocumentManagerPrivate::fileWatcher()
{
    if(!m_fileWatcher)
    {
        m_fileWatcher = new QFileSystemWatcher(m_instance);
        QObject::connect(m_fileWatcher, SIGNAL(fileChanged(QString)),
                         m_instance, SLOT(changedFile(QString)));
    }
    return m_fileWatcher;
}


}//namespace Internal;
}//namespace Core;
Q_DECLARE_METATYPE(Core::Internal::OpenWithEntry)

using namespace Core;
using namespace Internal;

DocumentManager::DocumentManager(QMainWindow *mw)
    : QObject(mw)
{
    d = new DocumentManagerPrivate(mw);
    m_instance = this;
    connect(d->m_mainWindow, SIGNAL(windowActivated()),
            this, SLOT(mainWindowActivated()));
    connect(Core::ICore::instance(), SIGNAL(contextChanged(Core::IContext*,Core::Context)),
            this, SLOT(syncWithEditor(Core::IContext*)));

	readSettings();
}

DocumentManager::~DocumentManager()
{
}

void DocumentManager::mainWindowActivated()
{
    QTimer::singleShot(0, this, SLOT(checkForReload()));
}

void DocumentManager::checkForReload()
{
    if(d->m_changedFiles.isEmpty())
        return ;
    if(QApplication::activeWindow() != d->m_mainWindow)
        return ;

    //这个是做什么用的呢？
    if(d->m_blockActivated)
        return ;

    d->m_blockActivated = true;

    IDocument::ReloadSetting defaultBehavior = EditorManager::instance()->reloadSetting();
    Utils::ReloadPromptAnswer previousAnswer = Utils::ReloadCurrent;

    QList<IEditor*> editorsToClose;
    QMap<IDocument*, QString> documentsToSave;

    //先收集文件信息
    QMap<QString, FileStateItem> currentStates;//文件名--最新状态
    QMap<QString, IDocument::ChangeType> changeTypes;//文件名--modified类型
    QSet<IDocument *> changedIDocuments;//所有发生改变的IDocument

    foreach (const QString &fileName, d->m_changedFiles)
    {
        IDocument::ChangeType type = IDocument::TypeContents;
        FileStateItem state;
        QFileInfo fi(fileName);
        if(!fi.exists())//文件不存在了，说明是文件移除了
        {
            type = IDocument::TypeRemoved;
        }
        else
        {
            state.modifiedTime  = fi.lastModified();
            state.permissions   = fi.permissions();
        }
        currentStates.insert(fileName, state);
        changeTypes.insert(fileName, type);
        FileState fileState = d->m_states.value(fileName);//所有记录的该文件的状态
        //这里还有些疑问，这之间的关系是什么？
        foreach(IDocument *document, fileState.lastUpdatedStates.keys())
        {
            changedIDocuments.insert(document);
        }
    }

    d->m_changedFiles.clear();

    QSet<QString> expectedFileNames;
    foreach(const QString &fileName, d->m_expectedFileNames)
    {
        const QString fixedName = fixFileName(fileName, KeepLinks);
        expectedFileNames.insert(fixedName);
        const QString fixedResolvedName = fixFileName(fileName, ResolveLinks);
        if(fixedName != fixedResolvedName)
        {
            expectedFileNames.insert(fixedResolvedName);
        }
    }
    QStringList errorStrings;
    foreach(IDocument *document, changedIDocuments)
    {
        IDocument::ChangeTrigger trigger = IDocument::TriggerInternal;
        IDocument::ChangeType type = IDocument::TypePermissions;
        bool changed = false;
        foreach(const QString &fileName, d->m_documentsWithWatch.value(document))
        {
            if(!currentStates.contains(fileName))
            {
                continue;
            }

            FileStateItem currentState = currentStates.value(fileName);
            FileStateItem expectedState = d->m_states.value(fileName).expected;
            FileStateItem lastState = d->m_states.value(fileName).lastUpdatedStates.value(document);
            //没有发生变化
            if (lastState.modifiedTime == currentState.modifiedTime
                    && lastState.permissions == currentState.permissions)
                continue;
            changed = true;
            //这里不处理仅发生访问权限变化的文件
            if(lastState.modifiedTime == currentState.modifiedTime)
            {
                continue;
            }
            //????????????这里有疑问
            //当前状态的时间!=被记录的状态的时间 && 被记录的文件名不包含fileName
            if((currentState.modifiedTime != expectedState.modifiedTime)
                    && !expectedFileNames.contains(fileName))
            {
                trigger = IDocument::TriggerExternal;//外部改变的
            }

            IDocument::ChangeType fileChange = changeTypes.value(fileName);
            if (fileChange == IDocument::TypeRemoved)
            {
                type = IDocument::TypeRemoved;
            }
            else if (fileChange == IDocument::TypeContents
                     && type == IDocument::TypePermissions)
            {
                type = IDocument::TypeContents;
            }
        }
        //？？？疑问！！
        if(!changed) //当前IDocment没有改变
            continue;

        d->m_blockedIDocument = document;

        bool success = true;
        QString errorString;
        //这里一定发生了变化，先检查是内容还是permission
        if(type == IDocument::TypePermissions)
        {
            //仅仅是权限改变
            success = document->reload(&errorString, IDocument::FlagReload, IDocument::TypePermissions);
        }
        else if(defaultBehavior == IDocument::ReloadUnmodified
                && type == IDocument::TypeContents
                && !document->isModified())
        {
            //？？？？疑问
            //内容改变或被删除
            success = document->reload(&errorString,
                                       IDocument::FlagReload,
                                       type);
        }
        else if (defaultBehavior == IDocument::ReloadUnmodified
                 && type == IDocument::TypeRemoved
                 && !document->isModified())
        {
            editorsToClose << EditorManager::instance()->editorsForDocument(document);
        }
        else if (defaultBehavior == IDocument::IgnoreAll)
        {
             success = document->reload(&errorString, IDocument::FlagIgnore, type);
        }
        else
        {
            if(document->reloadBehavior(trigger, type) == IDocument::BehaviorSilent)
            {
                if(type == IDocument::TypeRemoved)
                    editorsToClose << EditorManager::instance()->editorsForDocument(document);
                else
                    success = document->reload(&errorString, IDocument::FlagReload, type);
            }
            else if(type == IDocument::TypeContents)
            {
                if(previousAnswer == Utils::ReloadNone)
                {
                    success = document->reload(&errorString, IDocument::FlagReload, IDocument::TypeContents);
                }
                else if(previousAnswer == Utils::ReloadAll)
                {
                    success = document->reload(&errorString, IDocument::FlagReload, IDocument::TypeContents);
                }
                else
                {
                    previousAnswer = Utils::reloadPrompt(document->fileName(), document->isModified(), QApplication::activeWindow());
                    switch (previousAnswer) {
                    case Utils::ReloadAll:
                    case Utils::ReloadCurrent:
                        success = document->reload(&errorString, IDocument::FlagReload, IDocument::TypeContents);
                        break;
                    case Utils::ReloadSkipCurrent:
                    case Utils::ReloadNone:
                        success = document->reload(&errorString, IDocument::FlagIgnore, IDocument::TypeContents);
                        break;
                    }
                }
            }
            else
            {
                switch (Utils::fileDeletedPrompt(document->fileName(), trigger == IDocument::TriggerExternal, QApplication::activeWindow())) {
                case Utils::FileDeletedSave:
                    documentsToSave.insert(document, document->fileName());
                    break;
                case Utils::FileDeletedSaveAs:
                {
                    const QString &saveFileName = getSaveAsFileName(document);
                    if (!saveFileName.isEmpty()) {
                        documentsToSave.insert(document, saveFileName);
                    }
                    break;
                }
                case Utils::FileDeletedClose:
                    editorsToClose << EditorManager::instance()->editorsForDocument(document);
                    break;
                }
            }
        }
        if (!success)
        {
            if (errorString.isEmpty())
                errorStrings << tr("Cannot reload %1").arg(QDir::toNativeSeparators(document->fileName()));
            else
                errorStrings << errorString;
        }

        removeFileInfo(document);
        addFileInfo(document);
        d->m_blockedIDocument = 0;
    }

    if (!errorStrings.isEmpty())
        QMessageBox::critical(d->m_mainWindow, tr("File Error"),
                              errorStrings.join(QLatin1String("\n")));


    EditorManager::instance()->closeEditors(editorsToClose, false);
    QMapIterator<IDocument *, QString> it(documentsToSave);
    while (it.hasNext())
    {
        it.next();
        saveDocument(it.key(), it.value());
        it.key()->checkPermissions();
    }

    d->m_blockActivated = false;
}

void DocumentManager::checkForNewFileName()
{
    IDocument *document = qobject_cast<IDocument *>(sender());

    if (document == d->m_blockedIDocument)
        return;
    TOTEM_ASSERT(document, return);
    TOTEM_ASSERT(d->m_documentsWithWatch.contains(document), return);


    removeFileInfo(document);
    addFileInfo(document);
}

void DocumentManager::documentDestroyed(QObject *obj)
{
    IDocument *document = static_cast<IDocument*>(obj);
    if (!d->m_documentsWithoutWatch.removeOne(document))
        removeFileInfo(document);
}

void DocumentManager::fileNameChanged(const QString &oldName, const QString &newName)
{
    IDocument *doc = qobject_cast<IDocument *>(sender());
    TOTEM_ASSERT(doc, return);
    if (doc == d->m_blockedIDocument)
        return;
    emit m_instance->documentRenamed(doc, oldName, newName);

}

void DocumentManager::changedFile(const QString &fileName)
{
    const bool bEmpty = d->m_changedFiles.isEmpty();

    if (d->m_states.contains(fileName))
        d->m_changedFiles.insert(fileName);

    if (bEmpty && !d->m_changedFiles.isEmpty())
    {
        QTimer::singleShot(200, this, SLOT(checkForReload()));
    }
}

void DocumentManager::syncWithEditor(Core::IContext *context)
{
    if (!context)
        return;

    Core::IEditor *editor = Core::EditorManager::currentEditor();
    if (editor && editor->widget() == context->widget()
            && !editor->isTemporary())
        setCurrentFile(editor->document()->fileName());
}

bool DocumentManager::saveDocument(IDocument *document,
                                   const QString &fileName,
                                   bool *isReadOnly)
{
    bool ret = true;
    QString effName = fileName.isEmpty() ? document->fileName() : fileName;
    expectFileChange(effName);
    bool addWatcher = removeDocument(document);

    QString errorString;
    if (!document->save(&errorString, fileName, false))
    {
        if (isReadOnly)
        {
            QFile ofi(effName);

            if (!ofi.open(QIODevice::ReadWrite) && ofi.open(QIODevice::ReadOnly)) {
                *isReadOnly = true;
                goto out;
            }
            *isReadOnly = false;
        }
        QMessageBox::critical(d->m_mainWindow, tr("File Error"),
                              tr("Error while saving file: %1").arg(errorString));
      out:
        ret = false;
    }

    addDocument(document, addWatcher);
    unexpectFileChange(effName);
    return ret;
}

QStringList DocumentManager::getOpenFileNames(const QString &filters,
                                              const QString pathIn, QString *selectedFilter)
{
    QString path = pathIn;
    if (path.isEmpty())
    {
        if (!d->m_currentFile.isEmpty())
            path = QFileInfo(d->m_currentFile).absoluteFilePath();
    }
    const QStringList files = QFileDialog::getOpenFileNames(d->m_mainWindow,
                                                      tr("Open File"),
                                                      path, filters,
                                                      selectedFilter);
    if (!files.isEmpty())
        setFileDialogLastVisitedDirectory(QFileInfo(files.front()).absolutePath());
    return files;
}

QString DocumentManager::getSaveFileName(const QString &title, const QString &pathIn, const QString &filter, QString *selectedFilter)
{
    const QString &path = pathIn.isEmpty() ? fileDialogInitialDirectory() : pathIn;
    QString fileName;
    bool repeat;
    do
    {
        repeat = false;
        fileName = QFileDialog::getSaveFileName(
            d->m_mainWindow, title, path, filter, selectedFilter, QFileDialog::DontConfirmOverwrite);
        if (!fileName.isEmpty()) {
            // If the selected filter is All Files (*) we leave the name exactly as the user
            // specified. Otherwise the suffix must be one available in the selected filter. If
            // the name already ends with such suffix nothing needs to be done. But if not, the
            // first one from the filter is appended.
            if (selectedFilter && *selectedFilter != QCoreApplication::translate(
                    "Core", Constants::ALL_FILES_FILTER)) {
                // Mime database creates filter strings like this: Anything here (*.foo *.bar)
                QRegExp regExp(QLatin1String(".*\\s+\\((.*)\\)$"));
                const int index = regExp.lastIndexIn(*selectedFilter);
                bool suffixOk = false;
                if (index != -1) {
                    const QStringList &suffixes = regExp.cap(1).remove(QLatin1Char('*')).split(QLatin1Char(' '));
                    foreach (const QString &suffix, suffixes)
                        if (fileName.endsWith(suffix)) {
                            suffixOk = true;
                            break;
                        }
                    if (!suffixOk && !suffixes.isEmpty())
                        fileName.append(suffixes.at(0));
                }
            }
            if (QFile::exists(fileName)) {
                if (QMessageBox::warning(d->m_mainWindow, tr("Overwrite?"),
                    tr("An item named '%1' already exists at this location. "
                       "Do you want to overwrite it?").arg(fileName),
                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
                    repeat = true;
                }
            }
        }
    } while (repeat);
    if (!fileName.isEmpty())
        setFileDialogLastVisitedDirectory(QFileInfo(fileName).absolutePath());
    return fileName;
}

QString DocumentManager::getSaveFileNameWithExtension(const QString &title, const QString &pathIn, const QString &filter)
{
    QString selected = filter;
    return getSaveFileName(title, pathIn, filter, &selected);
}

QString DocumentManager::getSaveAsFileName(IDocument *document, const QString &filter, QString *selectedFilter)
{
    if (!document)
        return QLatin1String("");
    QString absoluteFilePath = document->fileName();
    const QFileInfo fi(absoluteFilePath);
    QString fileName = fi.fileName();
    QString path = fi.absolutePath();
    if (absoluteFilePath.isEmpty())
    {
        fileName = document->suggestedFileName();
        const QString defaultPath = document->defaultPath();
        if (!defaultPath.isEmpty())
            path = defaultPath;
    }

    QString filterString;
    if(filter.isEmpty())
    {
        filterString = document->suffixType();
        selectedFilter = &filterString;
    }
    else
    {
        filterString = filter;
    }

    absoluteFilePath = getSaveFileName(tr("Save File As"),
        path + QDir::separator() + fileName,
        filterString,
        selectedFilter);
    return absoluteFilePath;
}

QList<IDocument *> DocumentManager::saveModifiedDocumentsSilently(const QList<IDocument *> &documents, bool *cancelled)
{
    return saveModifiedFilesHelper(documents, cancelled, true, QString());
}

QList<IDocument *> DocumentManager::saveModifiedDocuments(const QList<IDocument *> &documents, bool *cancelled, const QString &message, const QString &alwaysSaveMessage, bool *alwaysSave)
{
    return saveModifiedFilesHelper(documents, cancelled, false, message, alwaysSaveMessage, alwaysSave);
}

QString DocumentManager::fileDialogLastVisitedDirectory()
{
    return d->m_lastVisitedDirectory;
}

void DocumentManager::setFileDialogLastVisitedDirectory(const QString &directory)
{
    d->m_lastVisitedDirectory = directory;
}

QString DocumentManager::fileDialogInitialDirectory()
{
    if (!d->m_currentFile.isEmpty())
        return QFileInfo(d->m_currentFile).absolutePath();
    return d->m_lastVisitedDirectory;
}

DocumentManager::ReadOnlyAction DocumentManager::promptReadOnlyFile(const QString &fileName, QWidget *parent, bool displaySaveAsButton)
{
    QMessageBox msgBox(QMessageBox::Question, tr("File Is Read Only"),
                       tr("The file <i>%1</i> is read only.").arg(QDir::toNativeSeparators(fileName)),
                       QMessageBox::Cancel, parent);

    QPushButton *makeWritableButton =  msgBox.addButton(tr("Make Writable"), QMessageBox::AcceptRole);

    QPushButton *saveAsButton = 0;
    if (displaySaveAsButton)
        saveAsButton = msgBox.addButton(tr("Save As..."), QMessageBox::ActionRole);

    msgBox.setDefaultButton(makeWritableButton);
    msgBox.exec();

    QAbstractButton *clickedButton = msgBox.clickedButton();
    if (clickedButton == (QAbstractButton *)makeWritableButton)
        return RO_MakeWriteable;
    if (displaySaveAsButton && clickedButton == (QAbstractButton *)saveAsButton)
        return RO_SaveAs;
    return  RO_Cancel;
}

bool DocumentManager::removeDocument(IDocument *document)
{
    TOTEM_ASSERT(document, return false);

    bool addWatcher = false;
    // 返回document是否是处于监视状态
    if (!d->m_documentsWithoutWatch.removeOne(document))
    {
        addWatcher = true;
        removeFileInfo(document);
        disconnect(document, SIGNAL(changed()), m_instance, SLOT(checkForNewFileName()));
    }
    disconnect(document, SIGNAL(destroyed(QObject*)), m_instance, SLOT(documentDestroyed(QObject*)));
    return addWatcher;
}

QList<IDocument *> DocumentManager::modifiedDocuments()
{
    QList<IDocument *> modified;

    foreach (IDocument *document, d->m_documentsWithWatch.keys())
    {
        if (document->isModified())
            modified << document;
    }

    foreach (IDocument *document, d->m_documentsWithoutWatch)
    {
        if (document->isModified())
            modified << document;
    }

    return modified;
}

void DocumentManager::renamedFile(const QString &from, const QString &to)
{
    const QString &fixedFrom = fixFileName(from, KeepLinks);

    QList<IDocument *> documentsToRename;
    QMapIterator<IDocument *, QStringList> it(d->m_documentsWithWatch);
    while (it.hasNext())
    {
        it.next();
        if (it.value().contains(fixedFrom))
            documentsToRename.append(it.key());
    }

    foreach (IDocument *document, documentsToRename)
    {
        d->m_blockedIDocument = document;
        removeFileInfo(document);
        document->rename(to);
        addFileInfo(document);
        d->m_blockedIDocument = 0;
    }
    emit m_instance->allDocumentsRenamed(from, to);
}

void DocumentManager::expectFileChange(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    d->m_expectedFileNames.insert(fileName);
}

static void updateExpectedState(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    if (d->m_states.contains(fileName))
    {
        QFileInfo fi(fileName);
        d->m_states[fileName].expected.modifiedTime = fi.lastModified();
        d->m_states[fileName].expected.permissions = fi.permissions();
    }
}

void DocumentManager::unexpectFileChange(const QString &fileName)
{
    if (fileName.isEmpty())
        return;
    d->m_expectedFileNames.remove(fileName);
    const QString fixedName = fixFileName(fileName, KeepLinks);
    updateExpectedState(fixedName);
    const QString fixedResolvedName = fixFileName(fileName, ResolveLinks);
    if (fixedName != fixedResolvedName)
        updateExpectedState(fixedResolvedName);
}



void DocumentManager::addToRecentFiles(const QString &fileName, const Id &editorId)
{
    if (fileName.isEmpty())
        return;
    QString unifiedForm(fixFileName(fileName, KeepLinks));
    QMutableListIterator<RecentFile > it(d->m_recentFiles);

    while (it.hasNext())
    {
        RecentFile file = it.next();
        QString recentUnifiedForm(fixFileName(file.first, DocumentManager::KeepLinks));
        if (unifiedForm == recentUnifiedForm)
            it.remove();
    }
    if (d->m_recentFiles.count() > d->m_maxRecentFiles)
        d->m_recentFiles.removeLast();
    d->m_recentFiles.prepend(RecentFile(fileName, editorId));

}

void DocumentManager::clearRecentFiles()
{
    d->m_recentFiles.clear();
}

QList<DocumentManager::RecentFile> DocumentManager::recentFiles()
{
    return d->m_recentFiles;
}

void DocumentManager::saveSettings()
{
    QStringList recentFiles;
    QStringList recentEditorIds;
    foreach (const RecentFile &file, d->m_recentFiles)
    {
        recentFiles.append(file.first);
        recentEditorIds.append(file.second.toString());
    }

    QSettings *s = Core::ICore::settings();
    s->beginGroup(QLatin1String(settingsGroupC));
    s->setValue(QLatin1String(filesKeyC), recentFiles);
    s->setValue(QLatin1String(editorsKeyC), recentEditorIds);
    s->endGroup();
}

void DocumentManager::readSettings()
{
	QSettings* pSettings = Core::ICore::settings();
	d->m_recentFiles.clear();
	pSettings->beginGroup(QLatin1String(settingsGroupC));
	QStringList files = pSettings->value(filesKeyC).toStringList();
	QStringList editors = pSettings->value(editorsKeyC).toStringList();
	pSettings->endGroup();
	QStringListIterator itrId(editors);
	foreach(QString f, files)
	{
		QString editorId;
		if (itrId.hasNext())
			editorId = itrId.next();
		if (QFileInfo(f).isFile())
			d->m_recentFiles.append(Core::DocumentManager::RecentFile(QDir::fromNativeSeparators(f), Core::Id(editorId)));
	}
}

void DocumentManager::setCurrentFile(const QString &filePath)
{
    if (d->m_currentFile == filePath)
        return;
    d->m_currentFile = filePath;
    emit m_instance->currentFileChanged(d->m_currentFile);

}

QString DocumentManager::currentFile()
{
    return d->m_currentFile;
}

QString DocumentManager::fixFileName(const QString &fileName, DocumentManager::FixMode fixmode)
{
    QString s = fileName;
    QFileInfo fi(s);
    if (fi.exists())
    {
        if (fixmode == ResolveLinks)
            s = fi.canonicalFilePath();
        else
            s = QDir::cleanPath(fi.absoluteFilePath());
    }
    else
    {
        s = QDir::cleanPath(s);
    }
    s = QDir::toNativeSeparators(s);
#ifdef Q_OS_WIN
    s = s.toLower();
#endif
    return s;
}

void DocumentManager::addDocuments(const QList<IDocument *> &documents, bool addWatcher)
{
    if (!addWatcher)
    {
        foreach (IDocument *document, documents)
        {
            if (document && !d->m_documentsWithoutWatch.contains(document))
            {
                connect(document, SIGNAL(destroyed(QObject*)), m_instance, SLOT(documentDestroyed(QObject*)));
                connect(document, SIGNAL(fileNameChanged(QString,QString)), m_instance, SLOT(fileNameChanged(QString, QString)));
                d->m_documentsWithoutWatch.append(document);
            }
        }
        return;
    }

    foreach (IDocument *document, documents)
    {
        if (document && !d->m_documentsWithWatch.contains(document))
        {
            connect(document, SIGNAL(changed()), m_instance, SLOT(checkForNewFileName()));
            connect(document, SIGNAL(destroyed(QObject*)), m_instance, SLOT(documentDestroyed(QObject*)));
            connect(document, SIGNAL(fileNameChanged(QString,QString)), m_instance, SLOT(fileNameChanged(QString, QString)));
            addFileInfo(document);
        }
    }
}

void DocumentManager::addDocument(IDocument *document, bool addWatcher)
{
    addDocuments(QList<IDocument *>() << document, addWatcher);
}

