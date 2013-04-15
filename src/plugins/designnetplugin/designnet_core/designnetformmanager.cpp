#include "coreplugin/editortoolbar.h"
#include "coreplugin/editormanager.h"
#include "coreplugin/minisplitter.h"
#include "coreplugin/icore.h"
#include "coreplugin/outputpaneplaceholder.h"
#include "coreplugin/imode.h"
#include "coreplugin/modemanager.h"
#include "coreplugin/actionmanager/command.h"
#include "coreplugin/actionmanager/actionmanager.h"
#include "coreplugin/actionmanager/actioncontainer.h"
#include "utils/runextensions.h"
#include "coreplugin/messagemanager.h"

#include "editordata.h"
#include "designnetformmanager.h"
#include "designnetmainwindow.h"
#include "designneteditor.h"
#include "designmode.h"
#include "designnetconstants.h"
#include "designnetcontext.h"
#include "designnetbase/designnetspace.h"

#include <QFutureWatcher>
#include <QVBoxLayout>
#include <QToolBar>
#include <QStyle>
using namespace Core;
namespace DesignNet{
QWidget *m_pWidgetTest = 0;
DesignNetFormManager *DesignNetFormManager::m_instance = 0;
DesignNetFormManager::DesignNetFormManager(QObject *parent) :
    QObject(parent)
{
    m_contexts.add(DesignNet::Constants::C_DESIGNNET);
    m_controlToolBar = 0;
    Core::ICore::addToolBar(m_controlToolBar);

    m_instance      = this;
    m_mainWindow    = new DesignNetMainWindow;
    m_toolBar       = 0;
    connect(ModeManager::instance(), SIGNAL(currentModeChanged(Core::IMode*)),
        SLOT(onModeChanged(Core::IMode*)));
    connect(Core::ICore::instance(), SIGNAL(coreAboutToOpen()), this, SLOT(onCoreAboutToOpen()));
    connect(EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor*)),
            this, SLOT(currentEditorChanged(Core::IEditor*)));
	connect(this, SIGNAL(logout(QString)), Core::ICore::messageManager(), SLOT(printToOutputPanePopup(const QString &)));
}

DesignNetFormManager::~DesignNetFormManager()
{
    if(m_modeWidget)
    {
        m_designMode->unregisterDesignWidget(m_modeWidget);
        delete m_modeWidget;
        m_modeWidget = 0;
    }
}

DesignNetFormManager *DesignNetFormManager::instance()
{
    return m_instance;
}

bool DesignNetFormManager::startInit()
{
    m_mainWindow->extensionsInitialized();

    m_editorToolBar = (QWidget*)createEditorToolBar();
    m_toolBar = Core::EditorManager::createToolBar();
    m_toolBar->addCenterToolBar(m_editorToolBar);
    createControlToolBar();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_toolBar);

    m_processWatcher = new QFutureWatcher<bool>();
    connect(m_processWatcher, SIGNAL(started()), this, SLOT(updateAction()));
    connect(m_processWatcher, SIGNAL(finished()), this, SLOT(updateAction()));
    updateAction();/// 根据m_processWatcher更新Action

    m_modeWidget    = new QWidget;
    m_modeWidget->setObjectName(QLatin1String("DesignNetModeWidget"));


    m_designMode = DesignNetMode::instance();


    Core::MiniSplitter *splitter = new Core::MiniSplitter(Qt::Vertical);
    splitter->addWidget(m_mainWindow);
    QWidget *outputPane = new Core::OutputPanePlaceHolder(m_designMode, splitter);
    outputPane->setObjectName(QLatin1String("DesignerOutputPanePlaceHolder"));
    splitter->addWidget(outputPane);
    layout->addWidget(splitter);
    m_modeWidget->setLayout(layout);

    Core::Context designNetContexts = m_contexts;
    m_context = new DesignNetContext(designNetContexts, m_modeWidget, this);
    Core::ICore::addContextObject(m_context);

    QStringList strSuffix;
    strSuffix << QLatin1String("txt");
    m_designMode->registerDesignWidget(m_modeWidget, strSuffix, m_contexts);
    return true;
}

DesignNetEditor *DesignNetFormManager::createEditor(QWidget *parent)
{
    DesignNetEditor *pEditor = new DesignNetEditor(parent);
    m_mainWindow->addEditor(pEditor);
    m_toolBar->addEditor(pEditor);
    return pEditor;
}

void DesignNetFormManager::writeSettings()
{
    m_mainWindow->writeSettings();
}

void DesignNetFormManager::readSettings()
{
    m_mainWindow->readSettings();
}

Command *DesignNetFormManager::addToolAction(QAction *a, const Context &context,
                                             const Core::Id &id,
                                             Core::ActionContainer *ac,
                                             const QString &keySequence)
{
    Core::Command *command = Core::ActionManager::registerAction(a, id, context);
    if (!keySequence.isEmpty())
        command->setDefaultKeySequence(QKeySequence(keySequence));
    ac->addAction(command);
    return command;
}

QToolBar *DesignNetFormManager::createEditorToolBar() const
{
    QToolBar *editorToolBar = new QToolBar;
    const QList<Core::Id>::const_iterator cend = m_toolActionIds.constEnd();
    for (QList<Core::Id>::const_iterator it = m_toolActionIds.constBegin(); it != cend; ++it)
    {
        Core::Command *cmd = Core::ActionManager::command(*it);
        QAction *action = cmd->action();
        if (!action->icon().isNull())
            editorToolBar->addAction(action);
    }
    const int size = editorToolBar->style()->pixelMetric(QStyle::PM_SmallIconSize);
    editorToolBar->setIconSize(QSize(size, size));
    editorToolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    return editorToolBar;
}

void DesignNetFormManager::createControlToolBar()
{
    m_controlToolBar = new QToolBar(tr("Process"));
    m_processAction = new QAction(tr("Start Process"), this);
    Command *cmd = ActionManager::registerAction(m_processAction,
                                 DesignNet::Constants::DESIGNNET_PROCESS_ID,
                                 m_contexts);
    cmd->setAttribute(Command::CA_Hide);
    m_controlToolBar->addAction(m_processAction);
    connect(m_processAction, SIGNAL(triggered(bool)), this, SLOT(onProcessAction(bool)));
    Core::ICore::addToolBar(m_controlToolBar);
}

void DesignNetFormManager::runSpace(QFutureInterface<bool> &fi, DesignNetSpace *space)
{
    fi.reportStarted();
    space->run(fi);
    fi.reportFinished();
}

void DesignNetFormManager::onModeChanged(Core::IMode *mode)
{
    m_mainWindow->onModeChanged(mode);
}

void DesignNetFormManager::onCoreAboutToOpen()
{
    m_mainWindow->onModeChanged(ModeManager::currentMode());
}

void DesignNetFormManager::currentEditorChanged(IEditor *editor)
{
    m_mainWindow->setVisibleEditor(editor);
    updateAction();
}

void DesignNetFormManager::onProcessAction(bool checked)
{
    Core::IEditor *editor = Core::EditorManager::currentEditor();
    if(editor)
    {
        DesignNetEditor *designNetEditor = qobject_cast<DesignNetEditor*>(editor);
        if(designNetEditor)
        {
            m_processWatcher->setFuture(QtConcurrent::run(&DesignNetFormManager::runSpace, this, designNetEditor->designNetSpace()));
		}
    }
}

void DesignNetFormManager::updateAction()
{
    Core::IEditor *editor = Core::EditorManager::currentEditor();
    DesignNetEditor *designNetEditor = 0;
    if(editor)
    {
        designNetEditor = qobject_cast<DesignNetEditor*>(editor);
    }
    m_processAction->setEnabled((!m_processWatcher->isRunning() && designNetEditor));
}

//void DesignNetFormManager::onActionsUpdate(IEditor *editor)
//{
//    DesignNetEditor *designNetEditor = qobject_cast<DesignNetEditor*>(editor);
//    if(designNetEditor)
//    {

//    }
//}

}
