#include "editmode.h"
#include "minisplitter.h"
#include "coreconstants.h"
#include "icorelistener.h"
#include "editormanager.h"
#include "ieditor.h"
#include "modemanager.h"
#include "minisplitter.h"
#include "outputpaneplaceholder.h"

#include <QVBoxLayout>
#include <QWidget>
namespace Core{

EditMode::EditMode()
    : m_rightSplitWidgetLayout(new QVBoxLayout)
{
    m_editorManager = EditorManager::instance();
    setObjectName(QLatin1String("EditMode"));
    setDisplayName(tr("Edit"));
    setIcon(QIcon(QLatin1String(":/core/images/mode_edit.png")));
    //设置模式优先级
    setPriority(Constants::P_MODE_EDIT);
    setId(Constants::MODE_EDIT);
    setType(Constants::MODE_EDIT_TYPE);

    m_rightSplitWidgetLayout->setSpacing(0);
    m_rightSplitWidgetLayout->setMargin(0);
    QWidget *rightSplitWidget = new QWidget;
    rightSplitWidget->setLayout(m_rightSplitWidgetLayout);
    m_rightSplitWidgetLayout->insertWidget(0,
                                           new Core::EditorManagerPlaceHolder(this));
    MiniSplitter *rightPaneSplitter = new MiniSplitter;
    rightPaneSplitter->insertWidget(0, rightSplitWidget);
    rightPaneSplitter->setStretchFactor(0, 1);

    m_splitter = new MiniSplitter;
    m_splitter->setOrientation(Qt::Vertical);
    m_splitter->insertWidget(0, rightPaneSplitter);

    QWidget *outputPane= new Core::OutputPanePlaceHolder(this, m_splitter);
    outputPane->setObjectName(QLatin1String("EditModeOutputPanePlaceHolder"));
    m_splitter->insertWidget(1, outputPane);
    m_splitter->setStretchFactor(0, 3);
    m_splitter->setStretchFactor(1, 0);

    connect(ModeManager::instance(), SIGNAL(currentModeChanged(Core::IMode*)),
            this, SLOT(grabEditorManager(Core::IMode*)));
    m_splitter->setFocusProxy(m_editorManager);
    setWidget(m_splitter);
}

EditMode::~EditMode()
{
    m_editorManager->setParent(0);
    delete m_splitter;
}

void EditMode::grabEditorManager(IMode *mode)
{
    if (mode != this)
        return;

    if (EditorManager::currentEditor())
        EditorManager::currentEditor()->widget()->setFocus();
}

}
