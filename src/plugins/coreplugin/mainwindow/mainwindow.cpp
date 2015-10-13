#include "stdafx.h"
#include "mainwindow.h"
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QPainter>
#include <QPushButton>
#include <QSettings>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QVBoxLayout>
#include "../actionmanager/actioncontainer.h"
#include "../actionmanager/actionmanager.h"
#include "../Common/icontext.h"
#include "../dialogs/newdialog.h"
#include "../dialogs/settingsdialog.h"
#include "../dialogs/shortcutsettings.h"
#include "../document/documentmanager.h"
#include "../document/editormanager.h"
#include "../constants.h"
#include "../icore.h"
#include "../iwizard.h"
#include "../version.h"
#include "extensionsystem/pluginmanager.h"
#include "rebarmanager.h"


static const char s_settingGroup[]		= "MainWindow";
static const char s_windowGeometryKey[] = "WindowGeometry";
static const char s_windowStateKey[]	= "WindowState";
using namespace Core;
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), m_settings(NULL),
	m_additionalContexts(Core::Constants::C_GLOBAL), m_activeContext(NULL),
	m_shortcutSettings(new Core::Internal::ShortcutSettings(this)),
	m_pDocManager(new Core::DocumentManager(this)),
	m_pEditorMgr(NULL)
{
	m_settings = ExtensionSystem::PluginManager::instance()->settings();
	setWindowTitle(ProductName);
	m_stack = new QStackedWidget(this);
}

MainWindow::~MainWindow()
{
	if (m_shortcutSettings)
	{
		ExtensionSystem::PluginManager::instance()->removeObject(m_shortcutSettings);
		delete m_shortcutSettings;
		m_shortcutSettings = 0;
	}
}

void MainWindow::init()
{
	connect(qApp, SIGNAL(focusChanged(QWidget*, QWidget*)),
		this, SLOT(updateFocusWidget(QWidget*, QWidget*)));
	updateContext();
	registerDefaultContainers();
	registerDefaultActions();
	readSettings();
	m_pEditorMgr = new Core::EditorManager(this);
	m_pEditorMgr->init();
	m_pEditorMgr->hide();
	QWidget* pMainContent = new QWidget(this);
	m_rebar = new RebarCtrl(this);
	m_pMainLayout = new QVBoxLayout();
	m_pMainLayout->addWidget(m_rebar);
	m_pMainLayout->addWidget(m_stack);
	pMainContent->setLayout(m_pMainLayout);
	setCentralWidget(pMainContent);
	ExtensionSystem::PluginManager::instance()->addObject(m_shortcutSettings);
}

void MainWindow::readSettings()
{
	if (m_settings)
	{
		m_settings->beginGroup(QLatin1String(s_settingGroup));
		if (restoreGeometry(m_settings->value(QLatin1String(s_windowGeometryKey)).toByteArray()))
			resize(1000, 700);
		restoreState(m_settings->value(QLatin1String(s_windowStateKey)).toByteArray());
		m_settings->endGroup();
	}
}

void MainWindow::writeSettings()
{
	if (m_settings)
	{
		m_settings->beginGroup(QLatin1String(s_settingGroup));
		m_settings->setValue(QLatin1String(s_windowGeometryKey), saveGeometry());
		m_settings->setValue(QLatin1String(s_windowStateKey), saveState());
		m_settings->endGroup();
	}
	Core::DocumentManager::saveSettings();
	Core::EditorManager::instance()->saveSettings();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
	writeSettings();
	e->accept();
	delete m_rebar;
	emit closed();
}

void MainWindow::registerDefaultContainers()
{
	Core::ActionManager* pMgr = Core::ActionManager::instance();
	if (!pMgr)
		return;

	Core::ActionContainer * pMenuBar = pMgr->createMenuBar(Core::Constants::MENU_BAR);
	if (pMenuBar)
	{
		setMenuBar(pMenuBar->menuBar());
		static struct
		{
			const char* pGroupId;
			const char* pMenuId;
			QString		strText;
		} menus[] = {
			{ Core::Constants::G_FILE, Core::Constants::M_FILE, tr("&File") },
			{ Core::Constants::G_TOOLS, Core::Constants::M_TOOLS, tr("&Tools") },
			{ Core::Constants::G_HELP, Core::Constants::M_HELP, tr("&Help") },
		};

		for (int i = 0; i < _countof(menus); ++i)
		{
			pMenuBar->appendGroup(menus[i].pGroupId);
			Core::ActionContainer* pContainer = pMgr->createMenu(menus[i].pMenuId);
			pMenuBar->addMenu(pContainer, menus[i].pGroupId);
			pContainer->menu()->setTitle(menus[i].strText);
		}
	}
}

void MainWindow::registerDefaultActions()
{
	Core::ActionManager* pMgr = Core::ActionManager::instance();
	if (!pMgr)
		return;
	Core::Context ctx(Core::Constants::C_GLOBAL);
	static struct
	{
		const char* sParentId;
		QString strTitle;
		QString strKeySequence;
		QString strIcon;
		const char* sMenuId;
		const char* sGroupId;
		const char* slotfunc;
	} actions[] = {
		{ Core::Constants::M_FILE, tr("New File..."), "Ctrl+N", "",
			Core::Constants::M_FILE_NEW, Core::Constants::G_FILE, SLOT(onNewFile()) },
		{ Core::Constants::M_FILE, tr("Open File..."), "Ctrl+O", "",
			Core::Constants::M_FILE_OPEN, Core::Constants::G_FILE, SLOT(onOpenFile()) },
		{ Core::Constants::M_FILE, tr("Save File..."), "Ctrl+S", "",
			Core::Constants::M_FILE_SAVE, Core::Constants::G_FILE, SLOT(onSaveFile()) },
		{ Core::Constants::M_FILE, tr("Save File As..."), "", "",
			Core::Constants::M_FILE_SAVEAS, Core::Constants::G_FILE, "" },
		{ Core::Constants::M_HELP, tr("About &Plugin"), tr("F12"), Core::Constants::ICON_ABOUT_PLUGINS,
			Core::Constants::M_HELP_ABOUTPLUGINS, Core::Constants::G_HELP, SLOT(aboutPlugins()) },
		{ Core::Constants::M_TOOLS, tr("&Option"), "", Core::Constants::ICON_OPTIONS_ACTION,
			Core::Constants::M_TOOLS, Core::Constants::G_TOOLS, SLOT(showOptionsDialog()) },
		{ Core::Constants::M_HELP, tr("About Totem"), tr("F1"), "",
			Core::Constants::M_HELP_ABOUT, Core::Constants::G_HELP, SLOT(aboutTotem()) },
	};

	for (int i = 0; i < _countof(actions); ++i)
	{
		Core::ActionContainer *pMenu = pMgr->actionContainer(actions[i].sParentId);
		if (pMenu)
		{
			QAction* pAction = new QAction(actions[i].strTitle, this);
			if (!actions[i].strIcon.isEmpty())
				pAction->setIcon(QIcon(actions[i].strIcon));
			Core::Command *cmd = pMgr->registerAction(pAction, actions[i].sMenuId, ctx);
			if (!actions[i].strKeySequence.isEmpty())
				cmd->setDefaultKeySequence(QKeySequence(actions[i].strKeySequence));
			pMenu->addAction(cmd);
			if (QString(actions[i].slotfunc).length())
				connect(pAction, SIGNAL(triggered(bool)), this, actions[i].slotfunc);
		}
	}
	ActionContainer* pContainer = pMgr->actionContainer(Core::Constants::M_FILE);
	ActionContainer* pOpen = pMgr->actionContainer(Core::Constants::M_FILE_OPEN);
	ActionContainer* pRecentFileMenu = pMgr->createMenu(Core::Constants::M_FILE_RECENT);
	pRecentFileMenu->menu()->setTitle("Recent Files...");
	pContainer->addMenu(pRecentFileMenu);
	connect(pRecentFileMenu, SIGNAL(aboutToShow()), this, SLOT(onAddRecentFilesMenu()));
}

void MainWindow::aboutTotem()
{

}

void MainWindow::aboutPlugins()
{

}

void MainWindow::updateFocusWidget(QWidget *old, QWidget *now)
{
	Q_UNUSED(old)

	// Prevent changing the context object just because the menu or a menu item is activated
	if (qobject_cast<QMenuBar*>(now) || qobject_cast<QMenu*>(now))
		return;

	Core::IContext *newContext = 0;
	if (focusWidget())
	{
		Core::IContext *context = 0;
		QWidget *p = focusWidget();
		while (p)
		{
			context = m_contextWidgets.value(p);
			if (context)
			{
				newContext = context;
				break;
			}
			p = p->parentWidget();
		}
	}
}

void MainWindow::updateContext()
{
	Core::Context contexts;
	contexts.add(m_additionalContexts);
	Core::Context uniquecontexts;
	for (int i = 0; i < contexts.size(); ++i)
	{
		const int c = contexts.at(i);
		if (!uniquecontexts.contains(c))
			uniquecontexts.add(c);
	}

	Core::ActionManager::setContext(uniquecontexts);
}

void MainWindow::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.fillRect(rect(), Qt::red);
}

void MainWindow::insertCenterWidget(QWidget* pWidget)
{
	m_stack->insertWidget(m_stack->count(), pWidget);
}

Core::IContext * MainWindow::contextObject(QWidget *widget)
{
	return m_contextWidgets.value(widget);
}

void MainWindow::addContextObject(Core::IContext *context)
{
	if (!context)
		return;

	QWidget *widget = context->widget();
	if (m_contextWidgets.contains(widget))
		return;

	m_contextWidgets.insert(widget, context);
}

void MainWindow::removeContextObject(Core::IContext *context)
{
	if (!context)
		return;

	QWidget *widget = context->widget();
	if (!m_contextWidgets.contains(widget))
		return;

	m_contextWidgets.remove(widget);
	if (m_activeContext == context)
		updateContextObject(0);
}

void MainWindow::resetContext()
{
	updateContextObject(0);
}

void MainWindow::updateContextObject(Core::IContext *context)
{
	if (context == m_activeContext)
		return;

	emit ICore::instance()->contextAboutToChange(context);
	m_activeContext = context;
	updateContext();
}

bool MainWindow::_showOptionsDialog(const QString &category, const QString &page, QWidget *parent)
{
	if (!parent)
		parent = this;
	Core::Internal::SettingsDialog *dialog = Core::Internal::SettingsDialog::getSettingsDialog(parent, category, page);
	return dialog->execDialog();
}

void MainWindow::onNewFile()
{
	Core::NewDialog dlg(this);
	dlg.setWizards(Core::IWizard::allWizards());
	Core::IWizard *wizard = dlg.showDialog();
	if (wizard)
	{
		QString path = Core::DocumentManager::fileDialogInitialDirectory();
		wizard->runWizard(path, this, QVariantMap());
	}
}

void MainWindow::onOpenLastUnclosed()
{

}

void MainWindow::onAddRecentFilesMenu()
{
	ActionContainer* pContainter = ActionManager::actionContainer(Core::Constants::M_FILE_RECENT);
	pContainter->menu()->clear();
	bool bHasRencentFile = false;
	foreach(const DocumentManager::RecentFile &file, DocumentManager::recentFiles())
	{
		QAction* pAction = pContainter->menu()->addAction(QDir::toNativeSeparators(file.first));
		pAction->setData(qVariantFromValue(file));
		connect(pAction, SIGNAL(triggered()), this, SLOT(onOpenFile()));
		bHasRencentFile = true;
	}
	pContainter->menu()->setEnabled(bHasRencentFile);
}

static QList<IDocumentFactory*> getNonEditorDocumentFactories()
{
	const QList<IDocumentFactory*> allFileFactories =
		ExtensionSystem::PluginManager::instance()->getObjects<IDocumentFactory>();
	QList<IDocumentFactory*> nonEditorFileFactories;
	foreach(IDocumentFactory *factory, allFileFactories)
	{
		if (!qobject_cast<IEditorFactory *>(factory))
			nonEditorFileFactories.append(factory);
	}
	return nonEditorFileFactories;
}

static IDocumentFactory *findDocumentFactory(const QList<IDocumentFactory*> &fileFactories,
	const QFileInfo &fi)
{
	const QString suffex = fi.completeSuffix();
	if (!suffex.isEmpty())
	{
		foreach(IDocumentFactory *factory, fileFactories) {
			if (factory->suffixTypes().contains(suffex))
				return factory;
		}
	}
	return 0;
}


void MainWindow::onOpenFile()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(0,
		"Select a file to open...",
		QDir::currentPath(), "Text (*.txt)");
	openFiles(fileNames);

}

void MainWindow::openFiles(const QStringList &fileNames)
{
	QList<IDocumentFactory*> nonEditorFileFactories = getNonEditorDocumentFactories();

	foreach(const QString &fileName, fileNames)
	{
		const QFileInfo fi(fileName);
		const QString absoluteFilePath = fi.absoluteFilePath();
		if (IDocumentFactory *documentFactory = findDocumentFactory(nonEditorFileFactories, fi))
		{
			Core::IDocument *document = documentFactory->open(absoluteFilePath);
			if (!document)
				return;
// 			if (document)
// 				ModeManager::activateMode(Id(Core::Constants::MODE_EDIT));
		}
		else
		{
			Core::IEditor *editor = EditorManager::openEditor(absoluteFilePath, Id());
			if (!editor)
				return;
		}
	}

}

void MainWindow::onSaveFile()
{
	DocumentManager::saveModifiedDocumentsSilently(DocumentManager::modifiedDocuments());
}
