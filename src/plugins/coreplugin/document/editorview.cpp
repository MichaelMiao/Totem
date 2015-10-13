#include "stdafx.h"
#include "editorview.h"
#include <QMouseEvent>
#include <QPainter>
#include <QSplitter>
#include <QStackedLayout>
#include <QStackedWidget>
#include "../icore.h"
#include "utils/totemassert.h"
#include "editormanager.h"
#include "idocument.h"
#include "ieditor.h"
#include "openeditorsmodel.h"



namespace Core{
namespace Internal{
EditorView::EditorView(QWidget *parent) :
    QWidget(parent),
    m_container(new QStackedWidget(this))
{
    QVBoxLayout *tl = new QVBoxLayout(this);
    tl->setSpacing(0);
    tl->setMargin(0);
    tl->addWidget(m_container);
}

EditorView::~EditorView()
{

}

int EditorView::editorCount() const
{
    return m_container->count();
}

void EditorView::addEditor(IEditor *editor)
{
    if (m_editors.contains(editor))
        return;

    m_editors.append(editor);

    m_container->addWidget(editor->widget());
    m_widgetEditorMap.insert(editor->widget(), editor);

    if (editor == currentEditor())
        setCurrentEditor(editor);
}

void EditorView::removeEditor(IEditor *editor)
{
    TOTEM_ASSERT(editor, return);
    if (!m_editors.contains(editor))
        return;
    //当前editor在QStackedWidget中的index
    const int index = m_container->indexOf(editor->widget());
    TOTEM_ASSERT((index != -1), return);
    //是否为当前editor
    bool wasCurrent = (index == m_container->currentIndex());
    m_editors.removeAll(editor);

    m_container->removeWidget(editor->widget());
    m_widgetEditorMap.remove(editor->widget());
    editor->widget()->setParent(0);

    if (wasCurrent)
        setCurrentEditor(m_editors.count() ? m_editors.last() : 0);

}

IEditor *EditorView::currentEditor() const
{
    if (m_container->count() > 0)
        return m_widgetEditorMap.value(m_container->currentWidget());
    return 0;
}

void EditorView::setCurrentEditor(IEditor *editor)
{
    if (!editor || m_container->count() <= 0
        || m_container->indexOf(editor->widget()) == -1)
    {
        return;
    }

    m_editors.removeAll(editor);
    m_editors.append(editor);

    const int idx = m_container->indexOf(editor->widget());
    m_container->setCurrentIndex(idx);
}

void EditorView::closeView()
{
	EditorManager *em = EditorManager::instance();
    IEditor *editor = currentEditor();
    if(editor)
        em->closeEditor(editor);
}

bool EditorView::hasEditor(IEditor *editor) const
{
    return m_editors.contains(editor);
}

QRect EditorView::editorArea() const
{
    const QRect cRect = m_container->rect();
    return QRect(m_container->mapToGlobal(cRect.topLeft()), cRect.size());
}

QList<IEditor *> EditorView::editors() const
{
    return m_widgetEditorMap.values();
}

SplitterOrView::SplitterOrView(OpenEditorsModel *model)
{
    m_isRoot    = true;
    m_layout    = new QStackedLayout(this);
    m_view      = new EditorView();
    m_splitter  = 0;
    m_layout->addWidget(m_view);
}

SplitterOrView::SplitterOrView(IEditor *editor)
{
    m_isRoot = false;
    m_layout = new QStackedLayout(this);
    m_view = new EditorView();
    if (editor)
        m_view->addEditor(editor);
    m_splitter = 0;
    m_layout->addWidget(m_view);
}

SplitterOrView::~SplitterOrView()
{
    delete m_layout;
    m_layout = 0;
    delete m_view;
    m_view = 0;
    delete m_splitter;
    m_splitter = 0;
}

void SplitterOrView::split(Qt::Orientation orientation)
{
}

void SplitterOrView::unsplit()
{
}

QSplitter *SplitterOrView::takeSplitter()
{
    QSplitter *oldSplitter = m_splitter;
    if (m_splitter)
        m_layout->removeWidget(m_splitter);
    m_splitter = 0;
    return oldSplitter;
}

EditorView *SplitterOrView::takeView()
{
    EditorView *oldView = m_view;
    if (m_view)
        m_layout->removeWidget(m_view);
    m_view = 0;
    return oldView;
}

QByteArray SplitterOrView::saveState() const
{
    return QByteArray();
}

void SplitterOrView::restoreState(const QByteArray &)
{
}

SplitterOrView *SplitterOrView::findView(IEditor *editor)
{
    if (!editor || hasEditor(editor))
        return this;
    if (m_splitter)
    {
        for (int i = 0; i < m_splitter->count(); ++i)
        {
            if (SplitterOrView *splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
                if (SplitterOrView *result = splitterOrView->findView(editor))
                    return result;
        }
    }
    return 0;
}

SplitterOrView *SplitterOrView::findView(EditorView *view)
{
    if (view == m_view)
        return this;
    if (m_splitter)
    {
        for (int i = 0; i < m_splitter->count(); ++i)
        {
            if (SplitterOrView *splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
                if (SplitterOrView *result = splitterOrView->findView(view))
                    return result;
        }
    }
    return 0;
}

SplitterOrView *SplitterOrView::findFirstView()
{
    if (m_splitter)
    {
        for (int i = 0; i < m_splitter->count(); ++i)
        {
            if (SplitterOrView *splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
                if (SplitterOrView *result = splitterOrView->findFirstView())
                    return result;
        }
        return 0;
    }
    return this;
}

SplitterOrView *SplitterOrView::findEmptyView()
{
    if (m_splitter)
    {
        for (int i = 0; i < m_splitter->count(); ++i)
        {
            if (SplitterOrView *splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
                if (SplitterOrView *result = splitterOrView->findEmptyView())
                    return result;
        }
        return 0;
    }
    if (!hasEditors())
        return this;
    return 0;
}

SplitterOrView *SplitterOrView::findSplitter(IEditor *editor)
{
    if (m_splitter)
    {
        for (int i = 0; i < m_splitter->count(); ++i)
        {
            if (SplitterOrView *splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
            {
                if (splitterOrView->hasEditor(editor))
                    return this;
                if (SplitterOrView *result = splitterOrView->findSplitter(editor))
                    return result;
            }
        }
    }
    return 0;
}

SplitterOrView *SplitterOrView::findSplitter(SplitterOrView *child)
{
    if (m_splitter)
    {
        for (int i = 0; i < m_splitter->count(); ++i)
        {
            if (SplitterOrView *splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
            {
                if (splitterOrView == child)
                    return this;
                if (SplitterOrView *result = splitterOrView->findSplitter(child))
                    return result;
            }
        }
    }
    return 0;
}

SplitterOrView *SplitterOrView::findNextView(SplitterOrView *view)
{
    bool found = false;
    return findNextView_helper(view, &found);
}

QSize SplitterOrView::minimumSizeHint() const
{
    if (m_splitter)
        return m_splitter->minimumSizeHint();
    return QSize(64, 64);
}

void SplitterOrView::unsplitAll()
{
    m_splitter->hide();
    m_layout->removeWidget(m_splitter);
    unsplitAll_helper();
    delete m_splitter;
    m_splitter = 0;
}

void SplitterOrView::paintEvent(QPaintEvent *)
{
    if (EditorManager::instance()->currentSplitterOrView() != this)
        return;

    if (!m_view || hasEditors())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().color(QPalette::Background).darker(107));

    const int r = 3;
    const QRect areaGlobal(view()->editorArea());
    const QRect areaLocal(mapFromGlobal(areaGlobal.topLeft()), areaGlobal.size());
    painter.drawRoundedRect(areaLocal.adjusted(r , r, -r, -r), r * 2, r * 2);

}

void SplitterOrView::mousePressEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton)
        return;
    setFocus(Qt::MouseFocusReason);
    EditorManager::instance()->setCurrentView(this);
}

void SplitterOrView::unsplitAll_helper()
{
    if (!m_isRoot && m_view)
		EditorManager::instance()->emptyView(m_view);
    if (m_splitter)
    {
        for (int i = 0; i < m_splitter->count(); ++i) {
            if (SplitterOrView *splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i))) {
                splitterOrView->unsplitAll_helper();
            }
        }
    }
}

SplitterOrView *SplitterOrView::findNextView_helper(SplitterOrView *view, bool *found)
{
    if (*found && m_view)
    {
        return this;
    }

    if (this == view)
    {
        *found = true;
        return 0;
    }

    if (m_splitter)
    {
        for (int i = 0; i < m_splitter->count(); ++i)
        {
            if (SplitterOrView *splitterOrView = qobject_cast<SplitterOrView*>(m_splitter->widget(i)))
            {
                if (SplitterOrView *result = splitterOrView->findNextView_helper(view, found))
                    return result;
            }
        }
    }
    return 0;
}

}
}
