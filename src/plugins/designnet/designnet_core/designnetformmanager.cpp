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
	: d(new DesignNetFormManagerPrivate())
{
	
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
	setupActions();

	d->m_modeWidget = new QWidget();
	d->m_modeWidget->setObjectName(QLatin1String("DesignNetModeWidget"));
	d->m_designMode = DesignNetMode::instance();

	d->m_mainWindow = new DesignNetMainWindow();
	d->m_mainWindow->initialize();
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
	return true;
}

QToolBar * DesignNetFormManager::createEditorToolBar() const
{
	QToolBar *editorToolBar = new QToolBar;
	const QList<Core::Id>::const_iterator cend = d->m_toolActionIds.constEnd();
	for (QList<Core::Id>::const_iterator it = d->m_toolActionIds.constBegin(); it != cend; ++it)
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
	d->m_toolActionIds.push_back(Core::Id("DesignNet.StartBuild"));
	
	
	QAction *pRunAction = new QAction(this);
	Command *pCommand = Core::ActionManager::registerAction(pRunAction, d->m_toolActionIds.back(), d->m_context);
	pCommand->setAttribute(Core::Command::CA_Hide);
	QIcon icon(":/media/start.png");
	pCommand->action()->setIcon(icon);
	QObject::connect(pRunAction, SIGNAL(triggered(bool)), this, SLOT(onRunDesignNet()));
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
