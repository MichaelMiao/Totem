#include "stdafx.h"
#include "editormanager.h"
#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QMessageBox>
#include <QPainter>
#include <QSet>
#include <QTimer>
#include <QVBoxLayout>
#include "../actionmanager/actionmanager.h"
#include "../mainwindow/mainwindow.h"
#include "../constants.h"
#include "../icore.h"
#include "../icorelistener.h"
#include "../version.h"
#include "extensionsystem/pluginmanager.h"
#include "utils/totemassert.h"
#include "documentmanager.h"
#include "editorview.h"
#include "ieditor.h"
#include "ieditorfactory.h"
#include "openeditorsmodel.h"


enum { debugEditorManager=0 };

static const char documentStatesKey[] = "EditorManager/DocumentStates";
static const char reloadBehaviorKey[] = "EditorManager/ReloadBehavior";
static const char autoSaveEnabledKey[] = "EditorManager/AutoSaveEnabled";
static const char autoSaveIntervalKey[] = "EditorManager/AutoSaveInterval";


static QString autoSaveName(const QString &fileName)
{
    return fileName + QLatin1String(".autosave");
}

template <class T>
T *findById(const Core::Id &id)
{
    const QList<T *> factories = ExtensionSystem::PluginManager::instance()->getObjects<T>();
    foreach(T *efl, factories)
        if (id == efl->id())
            return efl;
    return 0;
}
namespace Core{

namespace Internal{
class EditorClosingCoreListener : public ICoreListener
{
public:
    EditorClosingCoreListener(EditorManager *em);
    bool editorAboutToClose(IEditor *editor);
    bool coreAboutToClose();

private:
    EditorManager *m_em;
};

EditorClosingCoreListener::EditorClosingCoreListener(EditorManager *em)
    : m_em(em)
{

}

bool EditorClosingCoreListener::editorAboutToClose(IEditor *editor)
{
    return true;
}

bool EditorClosingCoreListener::coreAboutToClose()
{
    //�������˵������Ҫask for save��MainWindow::closeEvent�����������

    return m_em->closeAllEditors(true);
}
}// namespace Internal

//-------------------------------------------------
//---------EditorManager---------------------
EditorManagerPlaceHolder *EditorManagerPlaceHolder::m_current = 0;

EditorManagerPlaceHolder::EditorManagerPlaceHolder(QWidget *parent)
    : QWidget(parent)
{
    setLayout(new QVBoxLayout);
    layout()->setMargin(0);
}

EditorManagerPlaceHolder::~EditorManagerPlaceHolder()
{
    if(m_current == this)
    {
        EditorManager::instance()->setParent(0);
        EditorManager::instance()->hide();
    }
}

EditorManagerPlaceHolder *EditorManagerPlaceHolder::current()
{
    return m_current;
}

using namespace Core;
using namespace Core::Internal;

struct EditorManagerPrivate
{
    explicit EditorManagerPrivate(QWidget *parent);
    ~EditorManagerPrivate();
    EditorView *m_view;
    SplitterOrView *m_splitter;
    QPointer<IEditor> m_currentEditor;
    QPointer<SplitterOrView> m_currentView;
    QTimer *m_autoSaveTimer;
    EditorClosingCoreListener *m_coreListener;

    // actions
    QAction *m_revertToSavedAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_closeCurrentEditorAction;
    QAction *m_closeAllEditorsAction;
    QAction *m_closeOtherEditorsAction;

    OpenEditorsModel *m_editorModel;

    IDocument::ReloadSetting m_reloadSetting;

    QString m_titleAddition;
    QMap<QString, QVariant> m_editorStates;


    bool m_autoSaveEnabled;
    int m_autoSaveInterval;
};

EditorManagerPrivate::EditorManagerPrivate(QWidget *parent)
    : m_view(0),
      m_splitter(0),
      m_autoSaveTimer(0),
      m_revertToSavedAction(new QAction(EditorManager::tr("Revert to Saved"), parent)),
      m_saveAction(0),
      m_saveAsAction(0),
      m_closeCurrentEditorAction(new QAction(EditorManager::tr("Close"), parent)),
      m_closeAllEditorsAction(new QAction(EditorManager::tr("Close All"), parent)),
      m_closeOtherEditorsAction(new QAction(EditorManager::tr("Close Others"), parent)),
      m_reloadSetting(IDocument::AlwaysAsk),
      m_autoSaveEnabled(true),
	  m_coreListener(0),
      m_autoSaveInterval(5)//�Զ������ʱ����
{
    m_editorModel = new OpenEditorsModel(parent);
}

EditorManagerPrivate::~EditorManagerPrivate()
{

}

static EditorManager *m_instance = 0;

EditorManager::EditorManager(QWidget *parent) :
    QWidget(parent),
    d(new EditorManagerPrivate(parent))
{
    m_instance = this;
    d->m_splitter = new SplitterOrView(d->m_editorModel);
    d->m_view = d->m_splitter->view();

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(d->m_splitter);
	setLayout(layout);

    d->m_autoSaveTimer = new QTimer(this);
    connect(d->m_autoSaveTimer, SIGNAL(timeout()), SLOT(autoSave()));
	connect(ICore::instance(), SIGNAL(contextAboutToChange(Core::IContext*)),
		this, SLOT(handleContextChange(Core::IContext*)));

	const Context editManagerContext(Core::Constants::C_EDITORMANAGER);
	// menu
 	d->m_saveAction = Core::ActionManager::command(Core::Constants::M_FILE_SAVE)->action();
	d->m_saveAsAction = Core::ActionManager::command(Core::Constants::M_FILE_SAVEAS)->action();
 //	ActionManager::registerAction(d->m_saveAction, Constants::SAVE, editManagerContext);
 	connect(d->m_saveAction, SIGNAL(triggered(bool)), this, SLOT(saveDocument()));
 	connect(d->m_saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveDocumentAsFromContextMenu()));
    updateActions();
}

EditorManager::~EditorManager()
{
    m_instance = 0;
    if(ICore::instance())
    {
        if(d->m_coreListener)
        {
            ExtensionSystem::PluginManager::instance()->removeObject(d->m_coreListener);
            delete d->m_coreListener;
        }
    }

    if(d->m_splitter)
    {
        delete d->m_splitter;
        d->m_splitter = 0;
    }
    delete d;
}

EditorManager *EditorManager::instance() { return m_instance; }

void EditorManager::init()
{
    //�����¼�����
    d->m_coreListener = new EditorClosingCoreListener(this);
    ExtensionSystem::PluginManager::instance()->addObject(d->m_coreListener);
}

IEditor *EditorManager::openEditor(const QString &fileName,
                                   const Id &editorId, bool *newEditor)
{
    return m_instance->openEditor(m_instance->currentEditorView(),
                                  fileName, editorId, newEditor);
}

void EditorManager::addEditor(IEditor *editor)
{
    if (!editor)
        return;
    ICore::instance()->addContextObject(editor);

    d->m_editorModel->addEditor(editor);
    const bool isTemporary = editor->isTemporary();
    const bool addWatcher = !isTemporary;//������ʱ�ļ�����Ҫ���Watcher
    //���document�������������ȥ���ظ�
    DocumentManager::addDocument(editor->document(), addWatcher);
    if (!isTemporary)
        DocumentManager::addToRecentFiles(editor->document()->fileName(),
                                          editor->id());
    emit editorOpened(editor);
}

void EditorManager::removeEditor(IEditor *editor)
{
    d->m_editorModel->removeEditor(editor);

    DocumentManager::removeDocument(editor->document());
    ICore::instance()->removeContextObject(editor);
}

bool EditorManager::closeAllEditors(bool askAboutModifiedEditors)
{
    //��Model��ɾ������editor
    //���������ʣ���model���Ƴ������е�Editor֮�󣬵���openedEditors()���Ƿ���
    //��ֵ�Ŷ���
    d->m_editorModel->removeAllRestoredEditors();
    if (closeEditors(openedEditors(), askAboutModifiedEditors))
    {
        return true;
    }
    return false;
}

void EditorManager::closeEditor(IEditor *editor)
{
    if (!editor)
        return;
    closeEditors(QList<IEditor *>() << editor);
}

bool EditorManager::closeEditors(const QList<IEditor *> &editorsToClose, bool askAboutModifiedEditors)
{
    if (editorsToClose.isEmpty())
        return true;    
    SplitterOrView *currentSplitterOrView = this->currentSplitterOrView();
    bool closingFailed = false;
    //Ҫ�ر�Editor������Ҫ������ЩICoreListener��Ȼ������Щlistener�ر�
    QList<IEditor*> acceptedEditors;

    const QList<ICoreListener *> listeners =
        ExtensionSystem::PluginManager::instance()->getObjects<ICoreListener>();
    foreach (IEditor *editor, editorsToClose)
    {
        bool editorAccepted = true;

        foreach (ICoreListener *listener, listeners)
        {
            if (!listener->editorAboutToClose(editor))
            {
                editorAccepted = false;
                closingFailed = true;
                break;
            }
        }
        //����رճɹ�������ӵ�acceptedEditors
        if (editorAccepted)
            acceptedEditors.append(editor);
    }
    //������ر�ʧ��
    if (acceptedEditors.isEmpty())
        return false;
    //�Ƿ񱣴��޸ĵ��ļ�
    if (askAboutModifiedEditors)
    {
        bool cancelled = false;
        QList<IDocument*> list =
                DocumentManager::saveModifiedDocuments(documentsForEditors(acceptedEditors), &cancelled);
        if (cancelled)
            return false;
        //�������û�йرյ�IDocument
        if (!list.isEmpty())
        {
            closingFailed = true;
            QSet<IEditor*> skipSet = editorsForDocuments(list).toSet();
            //�ѱ���ʧ�ܵ�IEditor��Ҫ�رյ�Editor��ɾ��
            acceptedEditors = acceptedEditors.toSet().subtract(skipSet).toList();
        }
    }

    if (acceptedEditors.isEmpty())
        return false;

    QList<EditorView*> closedViews;

    // remove the editors
    foreach (IEditor *editor, acceptedEditors)
    {
        emit editorAboutToClose(editor);
        //���Editor�Ѿ�����ĳ���ļ�����������ļ�������ʱ�ģ��ȱ�������״̬
        if (!editor->document()->fileName().isEmpty()
                && !editor->isTemporary())
        {
            QByteArray state = editor->saveState();
            if (!state.isEmpty())
                d->m_editorStates.insert(editor->document()->fileName(),
                                         QVariant(state));
        }
        //�Ƴ�model��documentManager�е�editor
        removeEditor(editor);
        //��View���Ƴ�
        if (SplitterOrView *view = d->m_splitter->findView(editor))
        {
            if (editor == view->view()->currentEditor())
                closedViews += view->view();
            view->view()->removeEditor(editor);
        }
    }
    //��Ҫ�رյ�view��������
    foreach (EditorView *view, closedViews)
    {
        IEditor *newCurrent = view->currentEditor();
        if (!newCurrent)//û����Editor���Ͳ���һ��
            newCurrent = pickUnusedEditor();

        if (newCurrent)//����п��Բ���IEditor
        {
            activateEditor(view, newCurrent);
        }
        else
        {
            QModelIndex idx = d->m_editorModel->firstRestoredEditor();
            if (idx.isValid())
            {
                activateEditorForIndex(view, idx);
            }
            else
            {
                const QList<IEditor *> editors = d->m_editorModel->editors();
                if (!editors.isEmpty())
                    activateEditor(view, editors.last());
            }
        }
    }

    emit editorsClosed(acceptedEditors);
    //ɾ���Ѿ��رյ�IEditor
    foreach (IEditor *editor, acceptedEditors)
    {
        delete editor;
    }
    //���ǰView��IEditor
    if (currentSplitterOrView)
    {
        if (IEditor *editor = currentSplitterOrView->editor())
            activateEditor(currentSplitterOrView->view(), editor);
    }
    //�����ǰ��¼��IEditor����Ҫ����Action��WindowTitle�����������ʣ�
    //ΪʲôҪ���ǰView��IEditor�أ�
    if (!currentEditor())
    {
        emit currentEditorChanged(0);
        updateActions();
        updateWindowTitle();
    }

    return !closingFailed;
}

OpenEditorsModel *EditorManager::openedEditorsModel() const
{
    return d->m_editorModel;
}
/*!
 * \brief EditorManager::makeFileWritable
 *  ʹ�ļ���д
 * \param document
 * \return �û�ѡ��Ĳ���
 */
MakeWritableResult EditorManager::makeFileWritable(IDocument *document)
{
    if (!document)
        return Failed;
    QString directory = QFileInfo(document->fileName()).absolutePath();
    const QString &fileName = document->fileName();

    switch (DocumentManager::promptReadOnlyFile(fileName, ICore::instance()->getFirstMainWindow(), document->isSaveAsAllowed())) {
    case DocumentManager::RO_MakeWriteable:
    {
        const bool permsOk = QFile::setPermissions(fileName, QFile::permissions(fileName) | QFile::WriteUser);
        if (!permsOk)
        {
            QMessageBox::warning(ICore::instance()->getFirstMainWindow(), tr("Cannot Set Permissions"),  tr("Cannot set permissions to writable."));
            return Failed;
        }
    }
        document->checkPermissions();
        return MadeWritable;
    case DocumentManager::RO_SaveAs :
        return saveDocumentAs(document) ? SavedAs : Failed;
    case DocumentManager::RO_Cancel:
        break;
    }
    return Failed;
}
//����IDocument����IEditor, IEditor --> IDocument --> file
QList<IEditor *> EditorManager::editorsForDocuments(QList<IDocument *> documents) const
{
    const QList<IEditor *> editors = openedEditors();
    QSet<IEditor *> found;
    foreach (IDocument *document, documents)
    {
        foreach (IEditor *editor, editors)
        {
            if (editor->document() == document && !found.contains(editor))
            {
                found << editor;
            }
        }
    }
    return found.toList();
}

void EditorManager::emptyView(EditorView *view)
{
    if (!view)
        return;
    QList<IEditor *> editors = view->editors();
    foreach (IEditor *editor, editors)
    {
        editors.removeAll(editor);
        view->removeEditor(editor);
    }
    emit editorsClosed(editors);
    foreach (IEditor *editor, editors)
    {
        delete editor;
    }
}

void EditorManager::saveSettings()
{
    SettingsDatabase *settings = ICore::settingsDatabase();
    settings->setValue(QLatin1String(documentStatesKey), d->m_editorStates);
    settings->setValue(QLatin1String(reloadBehaviorKey), d->m_reloadSetting);
    settings->setValue(QLatin1String(autoSaveEnabledKey), d->m_autoSaveEnabled);
    settings->setValue(QLatin1String(autoSaveIntervalKey), d->m_autoSaveInterval);
}

void EditorManager::readSettings()
{
    SettingsDatabase *settings = ICore::settingsDatabase();
    if (settings->contains(QLatin1String(documentStatesKey)))
        d->m_editorStates = settings->value(QLatin1String(documentStatesKey))
            .value<QMap<QString, QVariant> >();

    if (settings->contains(QLatin1String(reloadBehaviorKey)))
        d->m_reloadSetting = (IDocument::ReloadSetting)settings->value(QLatin1String(reloadBehaviorKey)).toInt();

    if (settings->contains(QLatin1String(autoSaveEnabledKey)))
    {
        d->m_autoSaveEnabled = settings->value(QLatin1String(autoSaveEnabledKey)).toBool();
        d->m_autoSaveInterval = settings->value(QLatin1String(autoSaveIntervalKey)).toInt();
    }
    updateAutoSave();
}

void EditorManager::setReloadSetting(IDocument::ReloadSetting behavior)
{
    d->m_reloadSetting = behavior;
}

IDocument::ReloadSetting EditorManager::reloadSetting()
{
    return d->m_reloadSetting;
}

bool EditorManager::hasEditor(const QString &fileName) const
{
    return !editorsForFileName(fileName).isEmpty();
}
//��ȡָ���ļ�������IEditor
QList<IEditor *> EditorManager::editorsForFileName(const QString &filename) const
{
    QList<IEditor *> found;
    QString fixedname = DocumentManager::fixFileName(filename,
                                                     DocumentManager::KeepLinks);
    foreach (IEditor *editor, openedEditors())
    {
        if (fixedname == DocumentManager::fixFileName(editor->document()->fileName(),
                                                      DocumentManager::KeepLinks))
            found << editor;
    }
    return found;
}

QList<IEditor *> EditorManager::editorsForDocument(IDocument *document) const
{
    QList<IEditor *> found;
    foreach (IEditor *editor, openedEditors())
    {
        if (editor->document() == document)
            found << editor;
    }
    return found;
}

IEditor *EditorManager::currentEditor()
{
    return m_instance->d->m_currentEditor;
}

EditorView *EditorManager::currentEditorView() const
{
    return currentSplitterOrView()->view();
}

QList<IEditor *> EditorManager::openedEditors() const
{
    return d->m_editorModel->editors();
}

void EditorManager::addDocumentToRecentFiles(IDocument *document)
{
    bool isTemporary = true;
    Id editorId;
    QList<IEditor *> editors = editorsForDocument(document);
    foreach (IEditor *editor, editors)
    {
        if (!editor->isTemporary())
        {
            editorId = editor->id();
            isTemporary = false;
            break;
        }
    }
    if (!isTemporary)
        DocumentManager::addToRecentFiles(document->fileName(), editorId);
}

void EditorManager::activateEditor(IEditor *editor)
{
    SplitterOrView *splitterOrView = m_instance->d->m_splitter->findView(editor);
    EditorView *view = (splitterOrView ? splitterOrView->view() : 0);
    // TODO an IEditor doesn't have to belong to a view, which makes this method a bit funny
    if (!view)
        view = m_instance->currentEditorView();
    m_instance->activateEditor(view, editor);
}

void EditorManager::handleContextChange(IContext *context)
{
    if(debugEditorManager)
        qDebug() << Q_FUNC_INFO;
    IEditor *editor = context ? qobject_cast<IEditor*>(context) : 0;
    if(editor)
    {
        setCurrentEditor(editor);
    }
    else
    {
        updateActions();
    }
}

bool EditorManager::saveDocument(IDocument *documentParam)
{
    IDocument *document = documentParam;
    if (!document && currentEditor())
        document = currentEditor()->document();
    if (!document)
        return false;

    document->checkPermissions();

    const QString &fileName = document->fileName();

    if (fileName.isEmpty())
        return saveDocumentAs(document);

    bool success = false;
    bool isReadOnly;

    // try saving, no matter what isReadOnly tells us
    success = DocumentManager::saveDocument(document, fileName, &isReadOnly);

    if (!success && isReadOnly)
    {
        MakeWritableResult answer =
                makeFileWritable(document);
        if (answer == Failed)
            return false;
        if (answer == SavedAs)
            return true;

        document->checkPermissions();

        success = DocumentManager::saveDocument(document);
    }

    if (success)
    {
        addDocumentToRecentFiles(document);
    }

    return success;
}

bool EditorManager::saveDocumentAs(IDocument *documentParam)
{
	return true;
    IDocument *document = documentParam;
    if (!document && currentEditor())
        document = currentEditor()->document();
    if (!document)
        return false;

    const QString &filter = documentParam->suffixType();
    QString selectedFilter = documentParam->suffixType();
    const QString &absoluteFilePath =
        DocumentManager::getSaveAsFileName(document, filter, &selectedFilter);

    if (absoluteFilePath.isEmpty())
        return false;

    if (absoluteFilePath != document->fileName()) {
        // close existing editors for the new file name
        const QList<IEditor *> existList = editorsForFileName(absoluteFilePath);
        if (!existList.isEmpty()) {
            closeEditors(existList, false);
        }
    }

    const bool success = DocumentManager::saveDocument(document, absoluteFilePath);
    document->checkPermissions();

    // TODO: There is an issue to be treated here. The new file might be of a different mime
    // type than the original and thus require a different editor. An alternative strategy
    // would be to close the current editor and open a new appropriate one, but this is not
    // a good way out either (also the undo stack would be lost). Perhaps the best is to
    // re-think part of the editors design.

    if (success)
        addDocumentToRecentFiles(document);

    updateActions();
    return success;
}

void EditorManager::makeCurrentEditorWritable()
{
    if (IEditor* curEditor = currentEditor())
        makeFileWritable(curEditor->document());
}

void EditorManager::autoSave()
{
}

void EditorManager::setCurrentEditor(IEditor *editor)
{
    //�Ȱ�View���Ƴ�
    if(editor)
        setCurrentView(0);
    //���õ�ǰ��Editor
    if(d->m_currentEditor == editor)
        return;

    d->m_currentEditor = editor;
    if(editor)//��editor���������ڵ�view��CurrentEditor��λ��
    {
        if(SplitterOrView *splitterOrView = d->m_splitter->findView(editor))
            splitterOrView->view()->setCurrentEditor(editor);
        //����Ҫ����editor����ʷpos����ʱ��ȥʵ����
    }
    updateActions();
    updateWindowTitle();
    emit currentEditorChanged(editor);
}

void EditorManager::setCurrentView(SplitterOrView *view)
{
    if (view == d->m_currentView)
        return;

    SplitterOrView *old = d->m_currentView;
    d->m_currentView = view;

    if (old)
        old->update();
    if (view)
        view->update();

    if (view && !view->editor())
        view->setFocus();

}

IEditor *EditorManager::activateEditor(EditorView *view, IEditor *editor)
{
    Q_ASSERT(view);
    // ���Ҫ�����editorΪ�գ����ҵ�ǰEditorΪ��
    if (!editor)
    {
        if (!d->m_currentEditor)
            setCurrentEditor(0);
        return 0;
    }

    editor = placeEditor(view, editor);
    setCurrentEditor(editor);
    if (isVisible())
        editor->widget()->setFocus();
    return editor;
}

void EditorManager::activateEditorForIndex(EditorView *view,
                                           const QModelIndex &index)
{
    Q_ASSERT(view);
    //��view����ʾĳ��ModelIndex������
    IEditor *editor = index.data(Qt::UserRole).value<IEditor*>();
    //������IEdior�Ѿ����ˣ�����view����ʾeditor��
    if (editor)
    {
        activateEditor(view, editor);
        return;
    }
    //��view�д�fileName�ļ�
    QString fileName = index.data(Qt::UserRole + 1).toString();
    Core::Id id = index.data(Qt::UserRole + 2).value<Core::Id>();
    if (!openEditor(view, fileName, id))
        d->m_editorModel->removeEditor(index);
}

void EditorManager::restoreEditorState(IEditor *editor)
{
    TOTEM_ASSERT(editor, return);
    QString fileName = editor->document()->fileName();
    editor->restoreState(d->m_editorStates.value(fileName).toByteArray());
}
/*!
 * ��view�д��ļ�fileName
*/
IEditor *EditorManager::openEditor(Core::Internal::EditorView *view,
                                   const QString &fileName,
                                   const Id &editorId,
                                   bool *newEditor)
{
    if (debugEditorManager)
        qDebug() << Q_FUNC_INFO << fileName << editorId.name();

    QString fn = fileName;
    //�ļ��������ǿ�
    if (fn.isEmpty())
        return 0;
    //��ΪnewEditor����ֵ
    if (newEditor)
        *newEditor = false;
    //fn������IEditor
    const QList<IEditor *> editors = editorsForFileName(fn);
    //�����IEditor��Ӧ������ļ�
    if (!editors.isEmpty())
    {
        IEditor *editor = editors.first();
        return activateEditor(view, editor);
    }

    QString realFn = autoSaveName(fn);
    QFileInfo fi(fn);
    QFileInfo rfi(realFn);//�Զ�������ļ�
    //Դ�ļ�������/�Զ�������ļ������ڡ�Դ�ļ��Ƚ��£��Ϳ��԰���ʱ�ļ���ɾ��
    if (!fi.exists() || !rfi.exists()
            || fi.lastModified() >= rfi.lastModified())
    {
        QFile::remove(realFn);
        realFn = fn;
    }

    IEditor *editor = createEditor(editorId, fn);
    // ���ָ��editorId��Editor�޷����ļ������ÿ��Դ򿪸��ļ���Editor��
    if (!editor)
        editor = createEditor(Id(), fn);
    if (!editor) // û���ܴ򿪵�
        return 0;
    //�������Ϊ�ȴ�״̬
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QString errorString;
    //���ļ�
    if (!editor->open(&errorString, fn, realFn))
    {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(ICore::instance()->getFirstMainWindow(), tr("File Error"), errorString);
        delete editor;
        return 0;
    }
    if (realFn != fn)
        editor->document()->setRestoredFrom(realFn);
    addEditor(editor);

    if (newEditor)
        *newEditor = true;

    IEditor *result = activateEditor(view, editor);
    if (editor == result)
        restoreEditorState(editor);

    QApplication::restoreOverrideCursor();
    return result;
}

/*!
 * \brief EditorManager::createEditor
 * \param editorId ָ��Editor��ID
 * \param fileName Ҫ�򿪵��ļ�
 * \return ��������IEditor
 */
IEditor *EditorManager::createEditor(const Id &editorId, const QString &fileName)
{
    if (debugEditorManager)
        qDebug() << Q_FUNC_INFO << editorId.name() << fileName;

    EditorFactoryList factories;
    if (!editorId.isValid())
    {
        const QFileInfo fileInfo(fileName);
        // ��ȡ�ļ���׺����Ŀǰ�汾��ʹ��MimeType
        QString suffexName = fileInfo.suffix();

        factories = editorFactories(suffexName);
    }
    else
    {
        if (IEditorFactory *factory = findById<IEditorFactory>(editorId))
            factories.push_back(factory);
    }
    if (factories.empty()) {
        qWarning("%s: unable to find an editor factory for the file '%s', editor Id '%s'.",
                 Q_FUNC_INFO, fileName.toUtf8().constData(), editorId.name().constData());
        return 0;
    }

    IEditor *editor = factories.front()->createEditor(m_instance);
    if (editor)
    {
        connect(editor, SIGNAL(changed()), m_instance, SLOT(handleEditorStateChange()));

        emit m_instance->editorCreated(editor, fileName);
    }
    return editor;
}

EditorManager::EditorFactoryList EditorManager::editorFactories(const QString &suffexType)
{
    EditorManager::EditorFactoryList rc;
    const EditorFactoryList allFactories = ExtensionSystem::PluginManager::instance()->getObjects<IEditorFactory>();
    foreach(IEditorFactory *factory, allFactories)
    {
        if(factory->suffixTypes().contains(suffexType))
            rc << factory;
    }
    return rc;
}

void EditorManager::handleEditorStateChange()
{
    //���²˵�
    updateActions();
    //�������״̬�ı��editorû��modified�����Ƴ��Զ�������ļ�
    IEditor *theEditor = qobject_cast<IEditor *>(sender());
    if (!theEditor->document()->isModified())
        theEditor->document()->removeAutoSaveFile();
    IEditor *currEditor = currentEditor();
    if (theEditor == currEditor)
    {
        updateWindowTitle();
        emit currentEditorStateChanged(currEditor);
    }
}

void EditorManager::updateAutoSave()
{
    if (d->m_autoSaveEnabled)
        d->m_autoSaveTimer->start(d->m_autoSaveInterval * (60 * 1000));
    else
        d->m_autoSaveTimer->stop();
}

void EditorManager::updateWindowTitle()
{
	QString windowTitle = ProductName;
    const QString dashSep = QLatin1String(" - ");
    if (!d->m_titleAddition.isEmpty())
    {
        windowTitle.prepend(d->m_titleAddition + dashSep);
    }
    IEditor *curEditor = currentEditor();
    if (curEditor)
    {
        QString editorName = curEditor->displayName();
        qDebug() << editorName;
        if (!editorName.isEmpty())
            windowTitle.prepend(editorName + dashSep);
        QString filePath = QFileInfo(curEditor->document()->fileName()).absoluteFilePath();
        if (!filePath.isEmpty())
            ICore::instance()->getFirstMainWindow()->setWindowFilePath(filePath);
    }
    else
    {
		ICore::instance()->getFirstMainWindow()->setWindowFilePath(QString());
    }
    qDebug() << windowTitle;
	ICore::instance()->getFirstMainWindow()->setWindowTitle(windowTitle);
}

IEditor *EditorManager::pickUnusedEditor() const
{
    //�����д򿪵�IEditor�У��ֱ������Ӧ��view��
    //������IEditorû��view����editor����������view�ĵ�ǰediotr���ͷ���
    foreach (IEditor *editor, openedEditors())
    {
        SplitterOrView *view = d->m_splitter->findView(editor);
        if (!view || view->editor() != editor)
            return editor;
    }
    return 0;
}

IEditor *EditorManager::placeEditor(EditorView *view, IEditor *editor)
{
    Q_ASSERT(view && editor);
    //�����ǰView����IEditor������IEditor�е�Document == editor->document
    if (view->currentEditor() && view->currentEditor()->document() == editor->document())
        editor = view->currentEditor();
    //editorҪ��viewû�����editor
    if (!view->hasEditor(editor))
    {
        //�ҵ�editorԭ�����ڵ�view
        if (SplitterOrView *sourceView = d->m_splitter->findView(editor))
        {
            //���View��ǰ��editor�������editor
            if (editor != sourceView->editor())
            {
                //������sourceView���Ƴ����editor�������editor��view��
                //����view�е�ǰ��Editor���ó�editor
                sourceView->view()->removeEditor(editor);
                view->addEditor(editor);
                view->setCurrentEditor(editor);
                //���sourceViewû����editor,����һ��û�����õ�editor
                if (!sourceView->editor())
                {
                    if (IEditor *replacement = pickUnusedEditor())
                    {
                        sourceView->view()->addEditor(replacement);
                    }
                }
                return editor;
            }
        }
        view->addEditor(editor);
    }
    return editor;
}

SplitterOrView *EditorManager::currentSplitterOrView() const
{
    SplitterOrView *view = d->m_currentView;
    //�����ǰ��IEditor�����ȷ���IEditor��view�����򷵻�SplitterOrView�ĵ�һ��View��
    //���һ��ViewҲû�У���ֱ�ӷ���SplitterOrView
    if (!view)
        view = d->m_currentEditor ?
               d->m_splitter->findView(d->m_currentEditor):
               d->m_splitter->findFirstView();
    if (!view)
        return d->m_splitter;
    return view;
}

QList<IDocument *> EditorManager::documentsForEditors(QList<IEditor *> editors) const
{
    QSet<IEditor *> handledEditors;//ȥ���ظ���
    QList<IDocument *> documents;
    foreach (IEditor *editor, editors)
    {
        if (!handledEditors.contains(editor))
        {
            documents << editor->document();
            handledEditors.insert(editor);
        }
    }
    return documents;
}

void EditorManager::updateActions()
{
    QString fName;
    IEditor *curEditor = currentEditor();
    int openedCount = openedEditors().count() + d->m_editorModel->restoredEditors().count();

    if (curEditor)//��ǰ��IEditor
    {
        if (!curEditor->document()->fileName().isEmpty())
        {
            QFileInfo fi(curEditor->document()->fileName());
            fName = fi.fileName();//�������ļ���
        }
        else
        {
            fName = curEditor->displayName();
        }
        //�Ƿ��ڡ���ǰIEditor��Ӧ��Document�����޸ģ�����ȴ��ֻ���ġ���״̬

        bool ww = curEditor->document()->isModified()
                && curEditor->document()->isFileReadOnly();
        //������ڴ洢��״̬��ww��һ�£�����Ҫ�޸�
        if (ww != curEditor->document()->hasWriteWarning())
        {
            curEditor->document()->setWriteWarning(ww);
        }
    }
    //���±�������ΪAction��״̬
    d->m_saveAction->setEnabled(curEditor != 0 && curEditor->document()->isModified());
    d->m_saveAsAction->setEnabled(curEditor != 0 && curEditor->document()->isSaveAsAllowed());
    d->m_revertToSavedAction->setEnabled(curEditor != 0
            && !curEditor->document()->fileName().isEmpty()
            && curEditor->document()->isModified());

    QString quotedName;
    if (!fName.isEmpty())
        quotedName = QLatin1Char('"') + fName + QLatin1Char('"');

    d->m_saveAsAction->setText(tr("Save %1 &As...").arg(quotedName));
    d->m_saveAction->setText(tr("&Save %1").arg(quotedName));
    d->m_revertToSavedAction->setText(tr("Revert %1 to Saved").arg(quotedName));

    d->m_closeCurrentEditorAction->setEnabled(curEditor != 0);
    d->m_closeCurrentEditorAction->setText(tr("Close %1").arg(quotedName));
    d->m_closeAllEditorsAction->setEnabled(openedCount > 0);
    d->m_closeOtherEditorsAction->setEnabled(openedCount > 1);
    d->m_closeOtherEditorsAction->setText((openedCount > 1 ? tr("Close All Except %1").arg(quotedName) : tr("Close Others")));
}

void EditorManager::saveDocumentAsFromContextMenu()
{
}

void EditorManager::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.fillRect(rect(), QColor(Qt::yellow));
}

}
