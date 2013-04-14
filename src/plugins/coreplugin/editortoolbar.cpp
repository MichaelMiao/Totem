#include "editortoolbar.h"
#include "coreconstants.h"
#include "editormanager.h"
#include "actionmanager/actionmanager.h"
#include "openeditorsmodel.h"
#include "utils/totemassert.h"
#include "ieditor.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QComboBox>

namespace Core{
struct EditorToolBarPrivate
{
    explicit EditorToolBarPrivate(QWidget *parent,
                                  EditorToolBar *q);
    QToolButton *m_closeEditorButton;
    Core::OpenEditorsModel* m_editorsModel;
    QWidget *m_activeToolBar;
    QWidget *m_toolBarPlaceholder;
    QWidget *m_defaultToolBar;
    QComboBox *m_editorList;
};

EditorToolBarPrivate::EditorToolBarPrivate(QWidget *parent, EditorToolBar *q)
    : m_closeEditorButton(new QToolButton),
      m_activeToolBar(0),
      m_toolBarPlaceholder(new QWidget),
      m_defaultToolBar(new QWidget(q))
{

}

EditorToolBar::EditorToolBar(QWidget *parent) :
    QWidget(parent),
    d(new EditorToolBarPrivate(parent, this))
{
    QHBoxLayout *toolBarLayout = new QHBoxLayout(this);
    toolBarLayout->setMargin(0);
    toolBarLayout->setSpacing(0);
    toolBarLayout->addWidget(d->m_defaultToolBar);
    d->m_toolBarPlaceholder->setLayout(toolBarLayout);
    d->m_toolBarPlaceholder->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    d->m_defaultToolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    d->m_activeToolBar = d->m_defaultToolBar;

    d->m_closeEditorButton->setAutoRaise(true);
    d->m_closeEditorButton->setIcon(QIcon(QLatin1String(Constants::ICON_CLOSE_DOCUMENT)));
    d->m_closeEditorButton->setEnabled(false);
    static int i = 0;
    QString str = tr("%1").arg(i++);
    d->m_closeEditorButton->setToolTip(str);
    d->m_toolBarPlaceholder->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);


    d->m_editorsModel = EditorManager::instance()->openedEditorsModel();

    QHBoxLayout *toplayout = new QHBoxLayout(this);
    toplayout->setSpacing(0);
    toplayout->setMargin(0);
    toplayout->addWidget(d->m_toolBarPlaceholder, 1); // Custom toolbar stretches
    toplayout->addWidget(d->m_closeEditorButton);

    setLayout(toplayout);

    connect(d->m_closeEditorButton, SIGNAL(clicked()), this, SLOT(closeEditor()), Qt::QueuedConnection);
}

EditorToolBar::~EditorToolBar()
{
}

void EditorToolBar::addEditor(IEditor *editor)
{
    TOTEM_ASSERT(editor, return);
    connect(editor, SIGNAL(changed()), this, SLOT(checkEditorStatus()));
    QWidget *toolBar = editor->toolBar();
    addCenterToolBar(toolBar);
    updateEditorStatus(editor);
}

void EditorToolBar::setCurrentEditor(IEditor *editor)
{
//    d->m_editorList->setCurrentIndex(d->m_editorsModel->indexOf(editor).row());

    updateToolBar(editor->toolBar());

    updateEditorStatus(editor);
}

void EditorToolBar::removeToolbarForEditor(IEditor *editor)
{
    TOTEM_ASSERT(editor, return);
    disconnect(editor, SIGNAL(changed()), this, SLOT(checkEditorStatus()));

    QWidget *toolBar = editor->toolBar();
    if (toolBar != 0)
    {
        if (d->m_activeToolBar == toolBar)
        {
            d->m_activeToolBar = d->m_defaultToolBar;
            d->m_activeToolBar->setVisible(true);
        }
        d->m_toolBarPlaceholder->layout()->removeWidget(toolBar);
        toolBar->setVisible(false);
        toolBar->setParent(0);
    }
}

void EditorToolBar::addCenterToolBar(QWidget *toolBar)
{
    if(!toolBar)
        return;
    toolBar->setVisible(false);
    d->m_toolBarPlaceholder->layout()->addWidget(toolBar);

    updateToolBar(toolBar);
}

void EditorToolBar::closeEditor()
{
    IEditor *current = EditorManager::currentEditor();
    if (!current)
        return;
    EditorManager::instance()->closeEditor(current);
    emit closeClicked();
}

void EditorToolBar::updateActionShortcuts()
{
    //    d->m_closeEditorButton->setToolTip(ActionManager::command(Constants::CLOSE)->stringWithAppendedShortcut(EditorManager::tr("Close Document")));
}

void EditorToolBar::checkEditorStatus()
{
    IEditor *editor = qobject_cast<IEditor *>(sender());
    IEditor *current = EditorManager::currentEditor();

    if (current == editor)
        updateEditorStatus(editor);
}

void EditorToolBar::updateToolBar(QWidget *toolBar)
{
    if (!toolBar)
        toolBar = d->m_defaultToolBar;
    if (d->m_activeToolBar == toolBar)
        return;
    toolBar->setVisible(true);
    d->m_activeToolBar->setVisible(false);
    d->m_activeToolBar = toolBar;

}

void EditorToolBar::updateEditorStatus(IEditor *editor)
{
    bool status = editor != 0;
    d->m_closeEditorButton->setEnabled(status);
    if(!status)
    {
        qDebug() << d->m_closeEditorButton->toolTip();
        this->setVisible(false);
    }
}


}
