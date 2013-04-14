#ifndef EDITORVIEW_H
#define EDITORVIEW_H


#include <QWidget>
#include <QList>
#include <QMap>
QT_BEGIN_NAMESPACE
class QStackedLayout;
class QStackedWidget;
class QToolButton;
class QSplitter;
QT_END_NAMESPACE

namespace Core{

class IEditor;
class EditorToolBar;
class InfoBarDisplay;
class IContext;

class OpenEditorsModel;
namespace Internal{

class EditorView : public QWidget
{
    Q_OBJECT
public:
    explicit EditorView(QWidget *parent = 0);
    virtual ~EditorView();

    int editorCount() const;
    void addEditor(IEditor *editor);
    void removeEditor(IEditor *editor);
    IEditor *currentEditor() const;
    void setCurrentEditor(IEditor *editor);

    bool hasEditor(IEditor *editor) const;

    QRect editorArea() const;

    QList<IEditor *> editors() const;

private slots:
    void closeView();
private:
    EditorToolBar *m_toolBar;

    QStackedWidget *m_container;
    InfoBarDisplay *m_infoBarDisplay;

    QMap<QWidget *, IEditor *> m_widgetEditorMap;
    QList<IEditor *> m_editors;
};

class SplitterOrView  : public QWidget
{
    Q_OBJECT
public:
    explicit SplitterOrView(Core::OpenEditorsModel *model); // creates a root splitter
    explicit SplitterOrView(Core::IEditor *editor);
    ~SplitterOrView();

    void split(Qt::Orientation orientation);
    void unsplit();

    inline bool isView() const { return m_view != 0; }
    inline bool isRoot() const { return m_isRoot; }

    inline bool isSplitter() const { return m_splitter != 0; }
    inline Core::IEditor *editor() const { return m_view ? m_view->currentEditor() : 0; }
    inline QList<Core::IEditor *> editors() const { return m_view ? m_view->editors() : QList<Core::IEditor*>(); }
    inline bool hasEditor(Core::IEditor *editor) const { return m_view && m_view->hasEditor(editor); }
    inline bool hasEditors() const { return m_view && m_view->editorCount() != 0; }
    inline EditorView *view() const { return m_view; }
    inline QSplitter *splitter() const { return m_splitter; }
    QSplitter *takeSplitter();
    EditorView *takeView();

    QByteArray saveState() const;
    void restoreState(const QByteArray &);

    SplitterOrView *findView(Core::IEditor *editor);
    SplitterOrView *findView(EditorView *view);
    SplitterOrView *findFirstView();
    SplitterOrView *findEmptyView();
    SplitterOrView *findSplitter(Core::IEditor *editor);
    SplitterOrView *findSplitter(SplitterOrView *child);

    SplitterOrView *findNextView(SplitterOrView *view);

    QSize sizeHint() const { return minimumSizeHint(); }
    QSize minimumSizeHint() const;

    void unsplitAll();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);

private:
    void unsplitAll_helper();
    SplitterOrView *findNextView_helper(SplitterOrView *view, bool *found);
    bool m_isRoot;
    QStackedLayout *m_layout;
    EditorView *m_view;
    QSplitter *m_splitter;
};

}
}


#endif // EDITORVIEW_H
