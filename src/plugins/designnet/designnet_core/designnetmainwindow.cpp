#include "designnetmainwindow.h"
#include <QFileInfo>
#include <QList>
#include <QStackedWidget>
#include <QTabBar>
#include <QToolBar>
#include <QVBoxLayout>
#include "../../coreplugin/actionmanager/actioncontainer.h"
#include "../../coreplugin/actionmanager/actionmanager.h"
#include "../../coreplugin/coreconstants.h"
#include "../../coreplugin/editormanager.h"
#include "../../coreplugin/editortoolbar.h"
#include "../../coreplugin/imode.h"
#include "../../coreplugin/minisplitter.h"
#include "../../coreplugin/outputpaneplaceholder.h"
#include "../../coreplugin/rightpane.h"
#include "CustomUI/dockwindow.h"
#include "utils/totemassert.h"
#include "availabledatawidget.h"
#include "designnetconstants.h"
#include "designnetdocument.h"
#include "designneteditor.h"
#include "designnetview.h"
#include "minimapnavigator.h"
#include "tooldockwidget.h"


#define DESIGNNET_MINIMAP_WIDTH		100


using namespace Core;
using namespace CustomUI;


namespace DesignNet{
class DesignNetMainWindowPrivate
{
public:
	DesignNetMainWindowPrivate() {
		m_eEditState = EditState_Move;
		m_viewsMenu = 0;
		m_statckedWidget = 0;
		m_pFileTabBar = 0;
		m_availableDataWidget = 0;
		m_toolBar = 0;
	}
	Core::ActionContainer *	m_viewsMenu;
	QStackedWidget*			m_statckedWidget;
	QHash<QString, QVariant> m_dockWidgetStates;
	QList<IEditor*>			m_editors;
	QTabBar*				m_pFileTabBar;
	ToolDockWidget*			m_toolDockWidget;   //!< 工具
	AvailableDataWidget*	m_availableDataWidget;
	QList<Command *>		m_cmdAboutToAdd;    //!< 要添加的菜单
	EditorToolBar*			m_toolBar;
	EditState				m_eEditState;

	MinimapNavigator*		m_pMinimap;
};

DesignNetMainWindow::DesignNetMainWindow(QWidget *parent)
	: CustomUI::BaseMainWindow(parent),
	d(new DesignNetMainWindowPrivate)
{
	d->m_statckedWidget = new QStackedWidget(parent);
	d->m_toolBar		= Core::EditorManager::createToolBar(this);
	d->m_pFileTabBar	= new QTabBar(this);
	d->m_pFileTabBar->setTabsClosable(true);
	d->m_pFileTabBar->setMovable(true);
	d->m_pFileTabBar->setExpanding(false);
	connect(d->m_pFileTabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabChanged(int)));
	connect(d->m_pFileTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(onTabClose(int)));
	d->m_toolBar->setCreationFlags(EditorToolBar::FlagsStandalone);
	setObjectName(QLatin1String("EditorWidget"));
	QWidget *pWidget = new QWidget(this);
	QVBoxLayout* pLayout =new QVBoxLayout(pWidget);
	pLayout->addWidget(d->m_pFileTabBar);
	pLayout->addWidget(d->m_toolBar);
	pLayout->addWidget(d->m_statckedWidget);
	pWidget->setLayout(pLayout);
	setCentralWidget(pWidget);
	setDocumentMode(true);
	setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
	QObject::connect(EditorManager::instance(), SIGNAL(editorAboutToClose(Core::IEditor*)), this, SLOT(onEditorClosed(Core::IEditor*)));
	d->m_pMinimap		= new MinimapNavigator(d->m_statckedWidget);
	resetToDefaultLayout();
}

DesignNetMainWindow::~DesignNetMainWindow()
{
	delete d;
}

void DesignNetMainWindow::addEditor(Core::IEditor *pEditor)
{
	DesignNetEditor *designNetEditor = qobject_cast<DesignNetEditor*>(pEditor);
	if(designNetEditor)
	{
		connect(designNetEditor, SIGNAL(changed()), this, SLOT(titleChanged()));
		if (pEditor->toolBar())
			d->m_toolBar->addEditor(pEditor);
		int iIndex = d->m_pFileTabBar->addTab(pEditor->document()->fileName());
		d->m_pFileTabBar->setTabData(iIndex, qVariantFromValue<void*>(pEditor));
		d->m_statckedWidget->addWidget(designNetEditor->designNetView());
		connect(pEditor->document(), SIGNAL(titleChanged(QString)), this, SLOT(titleChanged()));
		d->m_editors.append(pEditor);
	}
}

void DesignNetMainWindow::titleChanged()
{
	DesignNetDocument * doc = qobject_cast<DesignNetDocument *>(sender());
	if (doc)
	{
		QString str = "untitled";
		IEditor *pEditor = 0;
		QList<IEditor*>::iterator itr = d->m_editors.begin();
		while (itr != d->m_editors.end())
		{
			IEditor *e = *itr;
			if (e->document() == doc)
			{
				str = e->displayName();
				pEditor = e;
				break;
			}
			itr++;
		}
		
		if (pEditor)
		{
			for (int i = 0; i < d->m_pFileTabBar->count(); i++)
			{
				Core::IEditor* pTempEditor = (Core::IEditor*)d->m_pFileTabBar->tabData(i).value<void*>();
				if (pEditor == pTempEditor)
				{
					d->m_pFileTabBar->setTabToolTip(i, str);
					str = QFileInfo(str).fileName();
					if (doc->isModified())
						str += "*";
					d->m_pFileTabBar->setTabText(i, str);
					break;
				}
			}
		}
	}
}

void DesignNetMainWindow::initialize()
{
	d->m_viewsMenu = ActionManager::actionContainer(Core::Constants::M_VIEW);
	
	// Toolbar
	// 工具栏
	d->m_toolDockWidget = new ToolDockWidget(this);
	d->m_availableDataWidget = new AvailableDataWidget(this);
	struct DockWidgetInfo{
		QWidget*	pWidget;
		QString		strTitle;
		QString		strObjName;

	}Infos[] = 
	{
		{ d->m_toolDockWidget, tr("Tools"), DesignNet::Constants::DESIGN_DOCKNAME_TOOL },
		{ d->m_availableDataWidget, tr("Available Datas"), DesignNet::Constants::DESIGN_DOCKNAME_AVAILIABLEDATA },
	};
	int iCount = _countof(Infos);
	for (int i = 0; i < iCount; i++)
	{
		Infos[i].pWidget->setWindowTitle(Infos[i].strTitle);
		Infos[i].pWidget->setObjectName(Infos[i].strObjName);
		DockWindow *dock = createDockWidget(Infos[i].pWidget);
		connect(dock, SIGNAL(minMaxClicked(bool&)),
			d->m_toolDockWidget, SLOT(onMinMax(bool&)));

		connect(dock->toggleViewAction(),
			SIGNAL(triggered(bool)),
			this, SLOT(onDockVisibilityChange(bool)));
	}
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

DesignNet::EditState DesignNetMainWindow::getEditState()
{
	return d->m_eEditState;
}

void DesignNetMainWindow::addCenterToolBar( QToolBar* pToolBar )
{
	d->m_toolBar->addCenterToolBar(pToolBar);
}

void DesignNetMainWindow::onConnectAction(bool bChecked)
{
	QAction* pAction = (QAction*)sender();
	if (!bChecked)
		pAction->setChecked(true);
	setEditState(EditState_Link);
}

void DesignNetMainWindow::onMoveAction(bool bChecked)
{
	QAction* pAction = (QAction*)sender();
	if (!bChecked)
		pAction->setChecked(true);
	setEditState(EditState_Move);
}

void DesignNetMainWindow::onEditorChanged( Core::IEditor* pEditor )
{
	if (d->m_eEditState == EditState_Move)
		onMoveAction(true);
	else if (d->m_eEditState == EditState_Link)
		onConnectAction(true);
	DesignNetEditor* pDesignNetEditor = qobject_cast<DesignNetEditor*>(EditorManager::currentEditor());
	if (pDesignNetEditor)
	{
		connect(pDesignNetEditor->designNetView(), SIGNAL(showAvailiableData(Processor*)), this, SLOT(onShowAvailiableData(Processor*)));
		for (int i = 0; i < d->m_pFileTabBar->count(); i++)
		{
			Core::IEditor* pTempEditor = (Core::IEditor*)d->m_pFileTabBar->tabData(i).value<void*>();
			if (pEditor == pTempEditor)
			{
				d->m_pFileTabBar->setCurrentIndex(i);
				break;
			}
		}
		d->m_statckedWidget->setCurrentWidget(pDesignNetEditor->designNetView());
		d->m_pMinimap->raise();
		d->m_pMinimap->setEditor(pDesignNetEditor);
	}
}

void DesignNetMainWindow::setEditState(EditState eState)
{
	d->m_eEditState = eState;
	DesignNetEditor* pDesignNetEditor = qobject_cast<DesignNetEditor*>(EditorManager::currentEditor());
	if (pDesignNetEditor)
	{
		pDesignNetEditor->designNetView()->setFocus();
		pDesignNetEditor->designNetView()->setEditState(d->m_eEditState);
	}
}

void DesignNetMainWindow::onShowAvailiableData(Processor* processor)
{
	d->m_availableDataWidget->setAvailiableData(processor);
}

void DesignNetMainWindow::onTabChanged(int iIndex)
{
	Core::IEditor* pEditor = (Core::IEditor*)d->m_pFileTabBar->tabData(iIndex).value<void*>();
	EditorManager::activateEditor(pEditor);
}

void DesignNetMainWindow::onTabClose(int iIndex)
{
	Core::IEditor* pEditor = (Core::IEditor*)d->m_pFileTabBar->tabData(iIndex).value<void*>();
	EditorManager::instance()->closeEditor(pEditor);
}

void DesignNetMainWindow::onEditorClosed(Core::IEditor* pEditor)
{
	for (int i = 0; i < d->m_pFileTabBar->count(); i++)
	{
		if ((Core::IEditor*)(d->m_pFileTabBar->tabData(i).value<void*>()) == pEditor)
		{
			d->m_pFileTabBar->removeTab(i);
			d->m_editors.removeAll(pEditor);
			break;
		}
	}
}

void DesignNetMainWindow::resizeEvent(QResizeEvent* event)
{
	d->m_pMinimap->setGeometry(0, 0, DESIGNNET_MINIMAP_WIDTH, DESIGNNET_MINIMAP_WIDTH);
}

}
