#include "designnetmainwindow.h"
#include "coreplugin/imode.h"
#include "coreplugin/actionmanager/actioncontainer.h"
#include "coreplugin/actionmanager/actionmanager.h"
#include "coreplugin/coreconstants.h"
#include "coreplugin/rightpane.h"
#include "coreplugin/minisplitter.h"
#include "utils/totemassert.h"
#include "coreplugin/editormanager.h"
#include "coreplugin/outputpaneplaceholder.h"
#include "designneteditor.h"
#include "designnetview.h"
#include "designnetconstants.h"
#include "tooldockwidget.h"
#include "CustomUI/dockwindow.h"
#include "coreplugin/editortoolbar.h"

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QList>
#include <QToolBar>

using namespace Core;
using namespace CustomUI;
namespace DesignNet{
class DesignNetMainWindowPrivate
{
public:

	Core::ActionContainer *	m_viewsMenu;
	QStackedWidget*			m_statckedWidget;
	QHash<QString, QVariant> m_dockWidgetStates;
	QList<IEditor*>			m_editors;
	ToolDockWidget*			m_toolDockWidget;   //!< 工具
	QList<Command *>		m_cmdAboutToAdd;    //!< 要添加的菜单
	EditorToolBar*			m_toolBar;
};
DesignNetMainWindow::DesignNetMainWindow(QWidget *parent)
	: CustomUI::BaseMainWindow(parent),
	d(new DesignNetMainWindowPrivate)
{
	d->m_statckedWidget = new QStackedWidget(parent);
	d->m_toolBar		= Core::EditorManager::createToolBar(this);
	d->m_toolBar->setCreationFlags(EditorToolBar::FlagsStandalone);
	setObjectName(QLatin1String("EditorWidget"));
	QWidget *pWidget = new QWidget(this);
	QVBoxLayout* pLayout =new QVBoxLayout(pWidget);
	pLayout->addWidget(d->m_toolBar);
	pLayout->addWidget(d->m_statckedWidget);
	pWidget->setLayout(pLayout);
	setCentralWidget(pWidget);
	setDocumentMode(true);
	setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	
	resetToDefaultLayout();
}

DesignNetMainWindow::~DesignNetMainWindow()
{
	delete d;
}

void DesignNetMainWindow::addEditor( Core::IEditor *pEditor )
{
	DesignNetEditor *designNetEditor = qobject_cast<DesignNetEditor*>(pEditor);
	if(designNetEditor)
	{
		connect(designNetEditor, SIGNAL(changed()), this, SLOT(titleChanged()));
		if (pEditor->toolBar())
			d->m_toolBar->addEditor(pEditor);
		d->m_statckedWidget->addWidget(designNetEditor->designNetView());
		d->m_editors.append(pEditor);
	}
}

void DesignNetMainWindow::titleChanged()
{
	DesignNetEditor * editor = qobject_cast<DesignNetEditor *>(sender());
	if (editor)
	{
		int i = 0;
		QString str = "untitled";
		foreach(IEditor *e, d->m_editors)
		{
			if (e == editor)
			{
				str = editor->displayName();
				break;
			}
			i++;
		}
	}
	
}

void DesignNetMainWindow::initialize()
{
	// Toolbar
	// 工具栏
	d->m_toolDockWidget = new ToolDockWidget(this);
	d->m_toolDockWidget->show();

	d->m_toolDockWidget->setWindowTitle(tr("Tools"));
	d->m_toolDockWidget->setObjectName(DesignNet::Constants::DESIGN_DOCKNAME_TOOL);
	d->m_viewsMenu = ActionManager::actionContainer(Core::Constants::M_VIEW);
	DockWindow *dock = createDockWidget(d->m_toolDockWidget);
	connect(dock, SIGNAL(minMaxClicked(bool&)),
		d->m_toolDockWidget, SLOT(onMinMax(bool&)));

	connect(dock->toggleViewAction(),
		SIGNAL(triggered(bool)),
		this, SLOT(onDockVisibilityChange(bool)));

	addDelayedMenu();
}

void DesignNetMainWindow::resetToDefaultLayout()
{
	setTrackingEnabled(false);
	QList<DockWindow *> dockWidgetList = dockWidgets();
	foreach (DockWindow *dockWidget, dockWidgetList)
	{
		dockWidget->setFloating(false);
		removeDockWidget(dockWidget);
	}
	foreach (QDockWidget *dockWidget, dockWidgetList)
		dockWidget->show();

	setTrackingEnabled(true);
}

CustomUI::DockWindow * DesignNetMainWindow::createDockWidget( QWidget *widget, Qt::DockWidgetArea area /*= Qt::LeftDockWidgetArea*/ )
{
	DockWindow *dockWindow = addDockForWidget(widget, area);
	dockWindow->setProperty(DesignNet::Constants::DEFAULT_DOCK_AREA, area);
	Context context(DesignNet::Constants::C_DESIGNNET);
	QAction *actionDock = dockWindow->toggleViewAction();
	Command *cmd = ActionManager::registerAction(actionDock,
		Core::Id(QLatin1String("DesignNet.") + widget->objectName()), context);
	d->m_cmdAboutToAdd.append(cmd);
	cmd->setAttribute(Command::CA_Hide);
	connect(dockWindow->toggleViewAction(), SIGNAL(triggered(bool)),
		this, SLOT(updateDockWidgetSettings()));
	connect(dockWindow, SIGNAL(topLevelChanged(bool)),
		this, SLOT(updateDockWidgetSettings()));
	connect(dockWindow, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
		this, SLOT(updateDockWidgetSettings()));
	return dockWindow;
}

CustomUI::DockWindow * DesignNetMainWindow::dockWindow( const QString &objectName )
{
	foreach(DockWindow *dock, dockWidgets())
	{
		if(dock->objectName() == objectName)
			return dock;
	}
	return 0;
}

static bool sortCommands(Command *cmd1, Command *cmd2)
{
	return cmd1->action()->text() < cmd2->action()->text();
}

void DesignNetMainWindow::updateDockWidgetSettings()
{
	d->m_dockWidgetStates = getSettings();
}

void DesignNetMainWindow::onResetLayout()
{
	foreach(DockWindow *dock, dockWidgets())
	{
		dock->setFloating(false);
		this->removeDockWidget(dock);
	}
	foreach(DockWindow *dock, dockWidgets())
	{
		QVariant value = dock->property(DesignNet::Constants::DEFAULT_DOCK_AREA);
		if(value.isValid())
		{
			Qt::DockWidgetArea area = Qt::DockWidgetArea(value.toInt());
			this->addDockWidget(area, dock);
		}
	}
	DockWindow *toolDock = dockWindow(DesignNet::Constants::DESIGN_DOCKNAME_TOOL);
	toolDock->show();
	DockWindow *propertyDock = dockWindow(DesignNet::Constants::DESIGN_DOCKNAME_PROPERTY);
	propertyDock->show();
}

void DesignNetMainWindow::addDelayedMenu()
{
	qSort(d->m_cmdAboutToAdd.begin(), d->m_cmdAboutToAdd.end(),
		&sortCommands);
	foreach(Command* cmd, d->m_cmdAboutToAdd)
	{
		d->m_viewsMenu->addAction(cmd);
	}
}

void DesignNetMainWindow::addCenterToolBar( QToolBar* pToolBar )
{
	d->m_toolBar->addCenterToolBar(pToolBar);
}

}
