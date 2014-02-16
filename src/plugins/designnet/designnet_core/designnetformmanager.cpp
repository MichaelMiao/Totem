#include "designnetformmanager.h"

#include "designnetmainwindow.h"
#include "designneteditor.h"
#include "designnetmode.h"
#include "designnetconstants.h"
#include "designnetcontext.h"

#include "coreplugin/minisplitter.h"
#include "coreplugin/outputpaneplaceholder.h"
#include "coreplugin/icore.h"
#include "coreplugin/actionmanager/command.h"
#include "coreplugin/actionmanager/actionmanager.h"
#include "coreplugin/editortoolbar.h"
#include "coreplugin/editormanager.h"
#include <QToolBar>
#include <QAction>
#include <QStyle>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
using namespace Core;
namespace DesignNet{
class DesignNetFormManagerPrivate
{
public:
	DesignNetFormManagerPrivate();
	~DesignNetFormManagerPrivate();
	DesignNetMainWindow*		m_mainWindow;
	QWidget*					m_modeWidget;
	DesignNetMode*				m_designMode;
	Core::Context				m_context;
	DesignNetContext*			m_designNetContext;
	QList<Core::Id>				m_toolActionIds;
};

DesignNetFormManagerPrivate::DesignNetFormManagerPrivate()
{
	m_context.add(DesignNet::Constants::C_DESIGNNET);
}

DesignNetFormManagerPrivate::~DesignNetFormManagerPrivate()
{

}

DesignNetFormManager *DesignNetFormManager::m_instance = 0;

DesignNetFormManager::DesignNetFormManager()
{
	d = new DesignNetFormManagerPrivate();
}

DesignNetFormManager::~DesignNetFormManager()
{
	delete d;
}

DesignNetFormManager * DesignNetFormManager::instance()
{
	if (!m_instance)
	{
		m_instance = new DesignNetFormManager();
	}
	return m_instance;
}

void DesignNetFormManager::Release()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = 0;
	}
}

DesignNetEditor * DesignNetFormManager::createEditor( QWidget *parent )
{
	DesignNetEditor *pEditor = new DesignNetEditor(parent);
	d->m_mainWindow->addEditor(pEditor);

	return pEditor;
}

bool DesignNetFormManager::startInit()
{
	d->m_modeWidget = new QWidget();
	d->m_modeWidget->setObjectName(QLatin1String("DesignNetModeWidget"));
	d->m_designMode = DesignNetMode::instance();

	d->m_mainWindow = new DesignNetMainWindow();
	d->m_mainWindow->initialize();
	setupActions();
	d->m_mainWindow->addCenterToolBar(createEditorToolBar());
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setMargin(0);
	layout->setSpacing(0);

	
	Core::MiniSplitter *splitter = new Core::MiniSplitter(Qt::Vertical);
	splitter->addWidget(d->m_mainWindow);

	DesignNetMode *designNetMode = DesignNetMode::instance();
	QWidget *outputPane = new Core::OutputPanePlaceHolder(designNetMode, splitter);
	outputPane->setObjectName(QLatin1String("DesignerOutputPanePlaceHolder"));
	splitter->addWidget(outputPane);
	layout->addWidget(splitter);
	d->m_modeWidget->setLayout(layout);
	d->m_designNetContext = new DesignNetContext(d->m_context, splitter, this);
	Core::ICore::addContextObject(d->m_designNetContext);

	QStringList strSuffix;
	strSuffix << QLatin1String("txt");
	designNetMode->registerDesignWidget(d->m_modeWidget, strSuffix, d->m_context);
	connect(EditorManager::instance(), SIGNAL(currentEditorChanged(Core::IEditor *)), d->m_mainWindow, SLOT(onEditorChanged(Core::IEditor*)));
	return true;
}

QToolBar * DesignNetFormManager::createEditorToolBar() const
{
	QToolBar *editorToolBar = new QToolBar;
	for (QList<Core::Id>::iterator it = d->m_toolActionIds.begin(); it != d->m_toolActionIds.end(); ++it)
	{
		Core::Command *cmd = Core::ActionManager::instance()->command(*it);
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
	
}

void DesignNetFormManager::setupActions()
{
	d->m_toolActionIds.push_back(Core::Id(Constants::DESIGNNET_PROCESS_ID));
	
	QAction *pAction = new QAction(this);
	Command *pCommand = Core::ActionManager::registerAction(pAction, d->m_toolActionIds.back(), d->m_context);
	pCommand->setAttribute(Core::Command::CA_Hide);
	QIcon icon(":/media/start.png");
	pCommand->action()->setIcon(icon);
	QObject::connect(pAction, SIGNAL(triggered(bool)), this, SLOT(onRunDesignNet()));

	QActionGroup* pActionGroup = new QActionGroup(this);
	pActionGroup->setExclusive(true);
	d->m_toolActionIds.push_back(Core::Id(Constants::DESIGNNET_EDITSTATE_MOVE_ACTION));
	pAction = pActionGroup->addAction(QIcon(":/media/cursor_arrow.png"), tr(""));
	pAction->setCheckable(true);
	pAction->setChecked(true);
	pCommand = Core::ActionManager::registerAction(pAction, d->m_toolActionIds.back(), d->m_context);
	pCommand->setAttribute(Core::Command::CA_Hide);
	QObject::connect(pAction, SIGNAL(triggered(bool)), d->m_mainWindow, SLOT(onMoveAction(bool)));

	d->m_toolActionIds.push_back(Core::Id(Constants::DESIGNNET_EDITSTATE_LINK_ACTION));
	pAction = pActionGroup->addAction(QIcon(":/media/link.png"), tr(""));
	pAction->setCheckable(true);
	pCommand = Core::ActionManager::registerAction(pAction, d->m_toolActionIds.back(), d->m_context);
	pCommand->setAttribute(Core::Command::CA_Hide);

	QObject::connect(pAction, SIGNAL(triggered(bool)), d->m_mainWindow, SLOT(onConnectAction(bool)));
}

void DesignNetFormManager::addToolAction( QAction* pAction, const Core::Context& context, const Core::Id &id, Core::ActionContainer *pContainer, const QString& keySequence )
{
	
}

void DesignNetFormManager::onRunDesignNet()
{
	DesignNetEditor *pEditor = qobject_cast<DesignNetEditor *>(Core::EditorManager::instance()->currentEditor());
	if (pEditor)
		pEditor->run();
}
}
