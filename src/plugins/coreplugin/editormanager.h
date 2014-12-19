#ifndef EDITORMANAGER_H
#define EDITORMANAGER_H

#include "core_global.h"
#include "id.h"
#include "idocument.h"
#include "ieditorfactory.h"

#include <QWidget>
#include <QList>
#include <QMenu>
#include <QModelIndex>

namespace Core{

enum MakeWritableResult {
    MadeWritable,
    SavedAs,
    Failed
};

class IContext;
class IEditor;
class IMode;
class EditorToolBar;
class OpenEditorsModel;
struct EditorManagerPrivate;

namespace Internal {

class EditorView;
class SplitterOrView;

class EditorClosingCoreListener;

} //namespace Internal

// 编辑窗口
class CORE_EXPORT EditorManagerPlaceHolder : public QWidget
{
    Q_OBJECT
public:
    explicit EditorManagerPlaceHolder(Core::IMode *mode, QWidget *parent = 0);
    ~EditorManagerPlaceHolder();
    static EditorManagerPlaceHolder* current();
private slots:
    void currentModeChanged(Core::IMode *);
private:
    Core::IMode *m_mode;
    static EditorManagerPlaceHolder* m_current; //当前编辑窗口
};

class CORE_EXPORT EditorManager : public QWidget
{
    Q_OBJECT
public:
    explicit EditorManager(QWidget *parent = 0);
    virtual ~EditorManager();

    typedef QList<IEditorFactory *> EditorFactoryList;

    static EditorManager *instance();

    void init();

    static IEditor *openEditor(const QString &fileName,
                               const Id &editorId = Id(),
                               bool *newEditor = 0);

    void addEditor(IEditor *editor);
    void removeEditor(IEditor *editor);


    static EditorToolBar *createToolBar(QWidget *parent = 0);

    bool closeAllEditors(bool askAboutModifiedEditors = true);
    void closeEditor(IEditor *editor);
    bool closeEditors(const QList<IEditor *> &editorsToClose, bool askAboutModifiedEditors = true);


    OpenEditorsModel *openedEditorsModel() const;

    MakeWritableResult makeFileWritable(IDocument *document);
    QList<IEditor*> editorsForDocuments(QList<IDocument *> documents) const;

    void emptyView(Internal::EditorView *view);

    void saveSettings();
    void readSettings();


    //reload config
    void setReloadSetting(IDocument::ReloadSetting behavior);
    IDocument::ReloadSetting reloadSetting();

    bool hasEditor(const QString &fileName) const;
    QList<IEditor *> editorsForFileName(const QString &filename) const;
    QList<IEditor *> editorsForDocument(IDocument *document) const;

    static IEditor *currentEditor();
    Internal::EditorView *currentEditorView() const;

    QList<IEditor*> openedEditors() const;

    void addDocumentToRecentFiles(IDocument *document);
    static void activateEditor(IEditor *editor);
    void switchToPreferedMode();
signals:
    void editorAboutToClose(Core::IEditor *editor);
    void editorOpened(Core::IEditor *editor);
    void editorsClosed(QList<Core::IEditor *> editors);
    void currentEditorChanged(Core::IEditor *editor);
    void editorCreated(Core::IEditor *editor, const QString &fileName);
    void currentEditorStateChanged(Core::IEditor *editor);

public slots:
    void updateActions();
    void handleContextChange(Core::IContext *context);
    bool saveDocument(Core::IDocument *documentParam = 0);
    bool saveDocumentAs(Core::IDocument *documentParam = 0);
	void saveDocumentAsFromContextMenu();

    void makeCurrentEditorWritable();
    void autoSave();
    void handleEditorStateChange();

private:
    void setCurrentEditor(IEditor *editor);
    void setCurrentView(Internal::SplitterOrView *view);

    IEditor *activateEditor(Internal::EditorView *view, IEditor *editor);
    void activateEditorForIndex(Internal::EditorView *view, const QModelIndex &index);
    void restoreEditorState(IEditor *editor);

    IEditor *openEditor(Internal::EditorView *view, const QString &fileName,
        const Id &editorId = Id(), bool *newEditor = 0);
    static IEditor *createEditor(const Id &id = Id(), const QString &fileName = QString());
    static EditorFactoryList editorFactories(const QString &suffexType);




    void updateAutoSave();
    void updateWindowTitle();
    IEditor *pickUnusedEditor() const;
    IEditor *placeEditor(Internal::EditorView *view, IEditor *editor);
    Internal::SplitterOrView *currentSplitterOrView() const;
    QList<IDocument *> documentsForEditors(QList<IEditor *> editors) const;

    EditorManagerPrivate *d;
    friend class Core::Internal::SplitterOrView;
    friend class Core::Internal::EditorView;
    friend class Core::EditorToolBar;

};
}

#endif // EDITORMANAGER_H
