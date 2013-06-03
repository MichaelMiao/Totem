#include "mainwindow.h"
#include "settingsdatabase.h"
#include "extensionsystem/pluginmanager.h"
#include "actionmanager/actionmanager.h"
#include "actionmanager/actionmanagerprivate.h"
#include "coreconstants.h"
#include "actionmanager/actioncontainerprivate.h"
#include "plugindialog.h"
#include "outputpanemanager.h"
#include "statusbarmanager.h"
#include "statusbarwidget.h"
#include "messagemanager.h"
#include "minisplitter.h"
#include "outputpaneplaceholder.h"
#include "dialogs/settingsdialog.h"
#include "shortcutsettings.h"
#include "rightpane.h"
#include "modewidget.h"
#include "modemanager.h"
#include "utils/stylehelper.h"
#include "editormanager.h"
#include "documentmanager.h"
#include "progressview.h"
#include "extensionsystem/progressmanagerprivate.h"
#include "newdialog.h"
#include "iwizard.h"
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QTreeView>
#include <QListView>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QFileDialog>
#include <QApplication>
using namespace Core;
using namespace Core::Internal;
using namespace ExtensionSystem;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_coreImpl(new ICore(this)),
      m_additionalContexts(Constants::C_GLOBAL),
      m_settings(ExtensionSystem::PluginManager::instance()->settings()),
      m_globalSettings(ExtensionSystem::PluginManager::instance()->globalSettings()),
      m_settingsDatabase(new SettingsDatabase(QFileInfo(m_settings->fileName()).path(),
                                                                      QLatin1String("QtCreator"),
                                                                      this)),
      m_actionManager(new ActionManager(this)),
      m_editorManager(0),
      m_shortcutSettings(new ShortcutSettings),
      m_outputView(0),
      m_activeContext(0),
      m_rightPaneWidget(0),
      m_messageManager(0),
      m_toggleSideBarButton(new QToolButton),
      m_progressManager(ProgressManagerPrivate::instance())

{
    new DocumentManager(this);
    OutputPaneManager::create();
    setDockNestingEnabled(true);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);



    registerDefaultContainers();
    registerDefaultActions();

    m_rightPaneWidget = new RightPaneWidget();
    m_modeStack = new ModeTabWidget(this);
    m_modeManager = new ModeManager(this, m_modeStack);
    this->setCentralWidget(m_modeStack);

    m_statusBarManager  = new StatusBarManager(this);//初始化状态栏
    m_messageManager    = new MessageManager();      //消息管理器，outputpane
    m_editorManager = new EditorManager(this);
    m_editorManager->hide();

    connect(QApplication::instance(), SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(updateFocusWidget(QWidget*,QWidget*)));
    statusBar()->insertPermanentWidget(0, m_toggleSideBarButton);
	setWindowIcon(QIcon(":/core/images/totem.ico"));
}

MainWindow::~MainWindow()
{
    delete m_messageManager;
    m_messageManager = 0;
    OutputPaneManager::destroy();
    ExtensionSystem::PluginManager::instance()->removeObject(m_outputView);
    ExtensionSystem::PluginManager::instance()->removeObject(m_shortcutSettings);
    delete m_statusBarManager;
    m_statusBarManager = 0;
    ExtensionSystem::PluginManager::instance()->removeObject(m_coreImpl);

    delete m_shortcutSettings;
    m_shortcutSettings = 0;
    delete m_rightPaneWidget;
    m_rightPaneWidget = 0;
    delete m_editorManager;
    m_editorManager = 0;

    delete m_coreImpl;
    m_coreImpl = 0;

}

bool MainWindow::initialize(QString *errorMessage)
{
    ExtensionSystem::PluginManager::instance()->addObject(m_coreImpl);

    m_statusBarManager->init();
    m_modeManager->init();



    m_outputView = new Core::StatusBarWidget;
    m_outputView->setWidget(OutputPaneManager::instance()->buttonsWidget());
    m_outputView->setPosition(Core::StatusBarWidget::Second);
    ExtensionSystem::PluginManager::instance()->addObject(m_outputView);
    m_messageManager->init();

    ExtensionSystem::PluginManager::instance()->addObject(m_shortcutSettings);

    return true;
}

void MainWindow::extensionsInitialized()
{
    m_editorManager->init();

    OutputPaneManager::instance()->init();
    m_actionManager->d->initialize();
    m_statusBarManager->extensionsInitalized();

    readSettings();
    updateContext();
    emit m_coreImpl->coreAboutToOpen();
    show();
    emit m_coreImpl->coreOpened();
    return;
}

void MainWindow::aboutToShutdown()
{
}

IContext *MainWindow::contextObject(QWidget *widget)
{
    return m_contextWidgets.value(widget);
}

void MainWindow::addContextObject(IContext *context)
{
    if (!context)
        return;
    QWidget *widget = context->widget();
    if (m_contextWidgets.contains(widget))
        return;

    m_contextWidgets.insert(widget, context);
}

void MainWindow::removeContextObject(IContext *context)
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

QSettings *MainWindow::settings(QSettings::Scope scope) const
{
    if (scope == QSettings::UserScope)
        return m_settings;
    else
        return m_globalSettings;

}

IContext *MainWindow::currentContextObject() const
{
    return m_activeContext;
}

void MainWindow::updateAdditionalContexts(const Context &remove, const Context &add)
{
    foreach (const int context, remove)
    {
        if (context == 0)
            continue;

        int index = m_additionalContexts.indexOf(context);
        if (index != -1)
            m_additionalContexts.removeAt(index);
    }

    foreach (const int context, add)
    {
        if (context == 0)
            continue;

        if (!m_additionalContexts.contains(context))
            m_additionalContexts.prepend(context);
    }

    updateContext();
}

bool MainWindow::hasContext(int context) const
{
    return m_actionManager->hasContext(context);
}

bool MainWindow::isPresentationModeEnabled()
{
    return m_actionManager->isPresentationModeEnabled();
}

void MainWindow::setPresentationModeEnabled(bool bEnabled)
{
    m_actionManager->setPresentationModeEnabled(bEnabled);
}

void MainWindow::setOverrideColor(const QColor &color)
{
    m_overrideColor = color;
}

ActionManager *MainWindow::actionManager() const
{
    return m_actionManager;
}

EditorManager *MainWindow::editorManager() const
{
    return m_editorManager;
}

MessageManager *MainWindow::messageManager() const
{
    return m_messageManager;
}

QStatusBar *MainWindow::statusBar() const
{
    return m_modeStack->statusBar();
}

ProgressManager *MainWindow::progressManager() const
{
    return m_progressManager;
}

void MainWindow::updateContextObject(IContext *context)
{
    if (context == m_activeContext)
        return;
    emit m_coreImpl->contextAboutToChange(context);
    m_activeContext = context;
    updateContext();

}

void MainWindow::updateContext()
{
    Context contexts;

    if (m_activeContext)
        contexts.add(m_activeContext->context());

    contexts.add(m_additionalContexts);
    //删除重复的
    Context uniquecontexts;
    for (int i = 0; i < contexts.size(); ++i)
    {
        const int c = contexts.at(i);
        if (!uniquecontexts.contains(c))
            uniquecontexts.add(c);
    }

    m_actionManager->d->setContext(uniquecontexts);
    emit m_coreImpl->contextChanged(m_activeContext, m_additionalContexts);
}

void MainWindow::registerDefaultContainers()
{
    ActionManager *am = m_actionManager;

    ActionContainer *menubar = am->createMenuBar(Constants::MENU_BAR);


    setMenuBar(menubar->menuBar());

    menubar->appendGroup(Constants::G_FILE);
    menubar->appendGroup(Constants::G_VIEW);
    menubar->appendGroup(Constants::G_WINDOW);
    menubar->appendGroup(Constants::G_TOOLS);
    menubar->appendGroup(Constants::G_HELP);

    ActionContainer *ac = 0;
    //File
    ac = am->createMenu(Constants::M_FILE);
    menubar->addMenu(ac, Constants::G_FILE);
    ac->menu()->setTitle(tr("&File"));
    ac->appendGroup(Constants::G_FILE_NEW);
    ac->appendGroup(Constants::G_FILE_OPEN);
	ac->appendGroup(Constants::G_FILE_SAVE);
    //view
    ac = am->createMenu(Constants::M_VIEW);
    menubar->addMenu(ac, Constants::G_VIEW);
    ac->menu()->setTitle(tr("&View"));

    //window
    ac = am->createMenu(Constants::M_WINDOW);
    menubar->addMenu(ac, Constants::G_WINDOW);
    ac->menu()->setTitle(tr("&Window"));
    ac->appendGroup(Constants::G_WINDOW_PANES);
    //Tool
    ac = am->createMenu(Constants::M_TOOLS);
    menubar->addMenu(ac, Constants::G_TOOLS);
    ac->menu()->setTitle(tr("&Tools"));

    //help
    ac = am->createMenu(Constants::M_HELP);
    menubar->addMenu(ac, Constants::G_HELP);
    ac->menu()->setTitle(tr("&Help"));
    ac->appendGroup(Constants::G_HELP_ABOUT);
    ac->appendGroup(Constants::G_HELP_ABOUT_PLUGIN);

}
static Command *createSeparator(ActionManager *am, QObject *parent,
                                const Id &id, const Context &context)
{
    QAction *tmpaction = new QAction(parent);
    tmpaction->setSeparator(true);
    Command *cmd = am->registerAction(tmpaction, id, context);
    return cmd;
}

void MainWindow::registerDefaultActions()
{
    Core::ActionManager *am = Core::ICore::actionManager();
    QAction *tmpAction = 0;
    Context globalContext(Constants::C_GLOBAL);
    //------------File menu---------------
    ActionContainer *mFile = am->actionContainer(Constants::M_FILE);
        // New
    QIcon icon(Constants::ICON_NEW_FILE);
    tmpAction = new QAction(icon, tr("New File"), mFile);
    Core::Command *cmdNew = am->registerAction(tmpAction,
                       Constants::NEW, globalContext);
    cmdNew->setDefaultKeySequence(QKeySequence(tr("Ctrl+N")));
    mFile->addAction(cmdNew, Constants::G_FILE_NEW);
    connect(tmpAction, SIGNAL(triggered()), this, SLOT(newFile()));

    //Open
    icon = QIcon::fromTheme(QLatin1String("document-open"), QIcon(QLatin1String(Constants::ICON_OPEN_FILE)));
    tmpAction = new QAction(icon, tr("&Open File or Project..."), this);
    Core::Command *cmdOpen = am->registerAction(tmpAction, Constants::OPEN, globalContext);
    cmdOpen->setDefaultKeySequence(QKeySequence::Open);
    mFile->addAction(cmdOpen, Constants::G_FILE_OPEN);
    connect(tmpAction, SIGNAL(triggered()), this, SLOT(openFile()));
	
	///  Save
	// Save Action
	icon = QIcon::fromTheme(QLatin1String("document-save"), QIcon(QLatin1String(Constants::ICON_SAVE_FILE)));
	QAction *tmpaction = new QAction(icon, tr("&Save"), this);
	tmpaction->setEnabled(false);
	Command *cmdSave = ActionManager::registerAction(tmpaction, Constants::SAVE, globalContext);
	cmdSave->setDefaultKeySequence(QKeySequence::Save);
	cmdSave->setAttribute(Command::CA_UpdateText);
	cmdSave->setDescription(tr("Save"));
	mFile->addAction(cmdSave, Constants::G_FILE_SAVE);
	// Save As Action
	icon = QIcon::fromTheme(QLatin1String("document-save-as"));
	tmpaction = new QAction(icon, tr("Save &As..."), this);
	tmpaction->setEnabled(false);
	Command *cmdSaveAs = ActionManager::registerAction(tmpaction, Constants::SAVEAS, globalContext);
	cmdSaveAs->setAttribute(Command::CA_UpdateText);
	cmdSaveAs->setDescription(tr("Save As..."));
	mFile->addAction(cmdSaveAs, Constants::G_FILE_SAVE);


    //------------help menu---------------
    ActionContainer *mHelp = am->actionContainer(Constants::M_HELP);

        // About Totem
    tmpAction = new QAction(tr("About Totem"), this);
    Core::Command *cmd = am->registerAction(tmpAction,
                                            Constants::ABOUT_TOTEM,
                                            globalContext);
    cmd->setDefaultKeySequence(QKeySequence(tr("F1")));
    mHelp->addAction(cmd, Constants::G_HELP_ABOUT);
    connect(tmpAction, SIGNAL(triggered()), this, SLOT(aboutTotem()));

        // About Totem plugin
    tmpAction = new QAction(tr("About &Plugin"), this);
    tmpAction->setIcon(QIcon(QLatin1String(Constants::ICON_ABOUT_PLUGINS)));
    cmd = am->registerAction(tmpAction, Constants::ABOUT_PLUGINS, globalContext);
    mHelp->addAction(cmd, Constants::G_HELP_ABOUT_PLUGIN);
    connect(tmpAction, SIGNAL(triggered()), this, SLOT(aboutPlugins()));

    //------------Tools Menu--------------
    ActionContainer *mtools = am->actionContainer(Constants::M_TOOLS);

    mtools->appendGroup(Constants::G_TOOLS_OPTIONS);
    mtools->addSeparator(globalContext, Constants::G_TOOLS_OPTIONS);
    m_optionsAction = new QAction(tr("&Options..."), this);
    m_optionsAction->setIcon(QIcon(QLatin1String(Constants::ICON_OPTION)));
    cmd = am->registerAction(m_optionsAction, Constants::OPTIONS, globalContext);

    mtools->addAction(cmd, Constants::G_TOOLS_OPTIONS);
    connect(m_optionsAction, SIGNAL(triggered()), this, SLOT(showOptionsDialog()));

}
static const char colorKey[] = "Color";
static const char settingsGroup[] = "MainWindow";
static const char windowGeometryKey[] = "WindowGeometry";
static const char windowStateKey[] = "WindowState";

void MainWindow::readSettings()
{
    m_settings->beginGroup(QLatin1String(settingsGroup));

    if (m_overrideColor.isValid())
    {
        Utils::StyleHelper::setBaseColor(m_overrideColor);

        m_overrideColor = Utils::StyleHelper::baseColor();
    }
    else
    {
        Utils::StyleHelper::setBaseColor(
                m_settings->value(QLatin1String(colorKey),
                                  QColor(Utils::StyleHelper::DEFAULT_BASE_COLOR)).value<QColor>());
    }

    if (!restoreGeometry(m_settings->value(QLatin1String(windowGeometryKey)).toByteArray()))
    {
        resize(1008, 700); // 大小没有保存的情况下
    }
    restoreState(m_settings->value(QLatin1String(windowStateKey)).toByteArray());

    m_settings->endGroup();

    m_editorManager->readSettings();
//    m_navigationWidget->restoreSettings(m_settings);
//    m_rightPaneWidget->readSettings(m_settings);
}

void MainWindow::writeSettings()
{
    m_settings->beginGroup(QLatin1String(settingsGroup));

    if (!(m_overrideColor.isValid() && Utils::StyleHelper::baseColor() == m_overrideColor))
        m_settings->setValue(QLatin1String(colorKey), Utils::StyleHelper::requestedBaseColor());

    m_settings->setValue(QLatin1String(windowGeometryKey), saveGeometry());
    m_settings->setValue(QLatin1String(windowStateKey), saveState());

    m_settings->endGroup();

    DocumentManager::saveSettings();
    m_actionManager->d->saveSettings(m_settings);
    m_editorManager->saveSettings();
//    m_navigationWidget->saveSettings(m_settings);
}

void MainWindow::aboutTotem()
{
    QMessageBox box;
    box.exec();
    //QMessageBox::about(this, tr("About %1").arg(i), tr("  By Michael_BJFU"));
}

void MainWindow::aboutPlugins()
{
    PluginDialog dlg(this);
    dlg.exec();
}

void MainWindow::updateFocusWidget(QWidget *old, QWidget *now)
{
    Q_UNUSED(old)

    // Prevent changing the context object just because the menu or a menu item is activated
    if (qobject_cast<QMenuBar*>(now) || qobject_cast<QMenu*>(now))
        return;

    IContext *newContext = 0;
    if (focusWidget())
    {
        IContext *context = 0;
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
    updateContextObject(newContext);
}

bool MainWindow::showOptionsDialog(const QString &category,
                                   const QString &page,
                                   QWidget *parent)
{
    emit m_coreImpl->optionsDialogRequested();
    if (!parent)
        parent = this;
    SettingsDialog *dialog = SettingsDialog::getSettingsDialog(parent, category, page);
    return dialog->execDialog();
}

void MainWindow::newFile()
{
	NewDialog dlg(this);
	dlg.setWizards(IWizard::allWizards());
	IWizard *wizard = dlg.showDialog();
	if(wizard)
	{
		QString path = DocumentManager::fileDialogInitialDirectory();
		wizard->runWizard(path, this, QVariantMap());
	}
	return ;
}

void MainWindow::openFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(0,
                                  "Select a file to open...",
                                 QDir::currentPath(), "Text (*.txt)");
    openFiles(fileNames);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit m_coreImpl->saveSettingsRequested();

    writeSettings();

    event->accept();
}

static QList<IDocumentFactory*> getNonEditorDocumentFactories()
{
    const QList<IDocumentFactory*> allFileFactories =
        ExtensionSystem::PluginManager::instance()->getObjects<IDocumentFactory>();
    QList<IDocumentFactory*> nonEditorFileFactories;
    foreach (IDocumentFactory *factory, allFileFactories) {
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
        foreach (IDocumentFactory *factory, fileFactories) {
            if (factory->suffixTypes().contains(suffex))
                return factory;
        }
    }
    return 0;
}

void MainWindow::openFiles(const QStringList &fileNames)
{
    QList<IDocumentFactory*> nonEditorFileFactories = getNonEditorDocumentFactories();

    foreach (const QString &fileName, fileNames)
    {
        const QFileInfo fi(fileName);
        const QString absoluteFilePath = fi.absoluteFilePath();
        if (IDocumentFactory *documentFactory = findDocumentFactory(nonEditorFileFactories, fi))
        {
            Core::IDocument *document = documentFactory->open(absoluteFilePath);
            if (!document)
                return;
            if (document)
                ModeManager::activateMode(Id(Core::Constants::MODE_EDIT));
        }
        else
        {
            Core::IEditor *editor = EditorManager::openEditor(absoluteFilePath, Id());
            if (!editor)
                return;
        }
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    if (e->type() == QEvent::ActivationChange)
    {
        if (isActiveWindow())
        {
            emit windowActivated();
        }
    }
}

void MainWindow::addModeTabToolBar(QToolBar *toolbar)
{
    m_modeStack->addToolBar(toolbar);
}

void MainWindow::removeModeTabToolBar(QToolBar *toolbar)
{
    m_modeStack->removeToolBar(toolbar);
}
