#include "designnetmainwindow.h"
#include "CustomUI/dockwindow.h"
#include "coreplugin/imode.h"
#include "coreplugin/icore.h"
#include "coreplugin/actionmanager/actioncontainer.h"
#include "coreplugin/actionmanager/actionmanager.h"
#include "coreplugin/coreconstants.h"
#include "coreplugin/editormanager.h"
#include "coreplugin/minisplitter.h"
#include "coreplugin/outputpaneplaceholder.h"
#include "designnetconstants.h"
#include "utils/totemassert.h"

#include "coreplugin/editmode.h"
#include "coreplugin/ieditor.h"
#include "coreplugin/editortoolbar.h"
#include "designneteditor.h"
#include "tooldockwidget.h"
#include "propertylistwidget.h"
#include "designview.h"
#include "designnetspace.h"

#include <QEvent>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QTreeView>
#include <QList>

using namespace Core;
using namespace CustomUI;
const char dockWindowStates[] = "DesignNet.dockWindowStates";
namespace DesignNet{

class DesignNetMainWindowPrivate
{
public:
    DesignNetMainWindowPrivate(DesignNetMainWindow *q);
    ActionContainer*    m_viewAction;
    QStackedWidget*     m_stackedWidget;
    QList<IEditor*>     m_editors;
    QHash<QString, QVariant> m_dockWidgetStates;
    QList<Command *>    m_cmdAboutToAdd;    //!< 要添加的菜单
    DockWindowEventFilter m_filter;
    ToolDockWidget*     m_toolDockWidget;   //!< 工具
    PropertyListWidget* m_propertyListWidget;//!< 属性窗口
};

DesignNetMainWindowPrivate::DesignNetMainWindowPrivate(DesignNetMainWindow *q)
    : m_viewAction(0),
      m_stackedWidget(new QStackedWidget(q)),
      m_filter(q),
      m_toolDockWidget(0)
{

}
static DesignNetMainWindow *m_instance = 0;
DesignNetMainWindow::DesignNetMainWindow(QWidget *parent) :
    BaseMainWindow(parent),
    d(new DesignNetMainWindowPrivate(this))
{
    m_instance = this;
    connect(this, SIGNAL(resetLayout()), this, SLOT(onResetLayout()));
    this->setCentralWidget(d->m_stackedWidget);
    d->m_viewAction = ActionManager::actionContainer(Core::Constants::M_VIEW);
    setDocumentMode(true);//设置边框
    setDockNestingEnabled(true);//设置多种停靠方式

    setStyleSheet(" \
                  QMainWindow::separator { \
                  background-color: #333333;\
                  height: 1; \
                  width: 1; \
                  color: #ffffff; \
                  } \
                  ");
}

DesignNetMainWindow::~DesignNetMainWindow()
{
    delete d;
}

DesignNetMainWindow *DesignNetMainWindow::instance()
{
    return m_instance;
}

DockWindow* DesignNetMainWindow::createDockWidget(QWidget *widget, Qt::DockWidgetArea area)
{
    DockWindow *dockWindow = addDockForWidget(widget, area);
    dockWindow->setProperty(DesignNet::Constants::DEFAULT_DOCK_AREA, area);
    Context context(DesignNet::Constants::C_DESIGNNET);
    QAction *actionDock = dockWindow->toggleViewAction();
    Command *cmd = ActionManager::registerAction(actionDock,
                                  Core::Id(QLatin1String("DesignNet.") + widget->objectName()), context);
    d->m_cmdAboutToAdd.append(cmd);
    cmd->setAttribute(Command::CA_Hide);
    dockWindow->installEventFilter(&(d->m_filter));
    connect(dockWindow->toggleViewAction(), SIGNAL(triggered(bool)),
        this, SLOT(updateDockWidgetSettings()));
    connect(dockWindow, SIGNAL(topLevelChanged(bool)),
        this, SLOT(updateDockWidgetSettings()));
    connect(dockWindow, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)),
        this, SLOT(updateDockWidgetSettings()));
    return dockWindow;
}

DockWindow *DesignNetMainWindow::dockWindow(const QString &objectName)
{
    foreach(DockWindow *dock, dockWidgets())
    {
        if(dock->objectName() == objectName)
            return dock;
    }
    return 0;
}

void DesignNetMainWindow::setVisibleEditor(IEditor *xmlEditor)
{
    foreach(IEditor *pEditor, d->m_editors)
    {
        if(pEditor == xmlEditor)
        {
            DesignNetEditor *designNetEditor = qobject_cast<DesignNetEditor*>(pEditor);
            if(designNetEditor)
            {
                d->m_stackedWidget->setCurrentWidget(designNetEditor->designNetWidget());
            }
        }
    }
}

void DesignNetMainWindow::onModeChanged(IMode *mode)
{
    bool bVisual = mode && mode->id() == Constants::DESIGNNET_MODE;
    setDockActionsVisible(bVisual);
    if(bVisual)
    {
        readSettings();
    }
    else
    {
        foreach (QDockWidget *dockWidget, dockWidgets())
        {
            if (dockWidget->isFloating())
                dockWidget->hide();
        }
    }
}

void DesignNetMainWindow::updateDockWidgetSettings()
{
    d->m_dockWidgetStates = getSettings();
}

void DesignNetMainWindow::extensionsInitialized()
{
	
    // 工具栏
    d->m_toolDockWidget = new ToolDockWidget(this);
	d->m_toolDockWidget->show();
	
    d->m_toolDockWidget->setWindowTitle(tr("Tools"));
    d->m_toolDockWidget->setObjectName(DesignNet::Constants::DESIGN_DOCKNAME_TOOL);
    DockWindow *dock = createDockWidget(d->m_toolDockWidget);
    connect(dock, SIGNAL(minMaxClicked(bool&)),
            d->m_toolDockWidget, SLOT(onMinMax(bool&)));

    connect(dock->toggleViewAction(),
            SIGNAL(triggered(bool)),
            this, SLOT(onDockVisibilityChange(bool)));
    // 属性窗口
    d->m_propertyListWidget = new PropertyListWidget(this);
    d->m_propertyListWidget->setWindowTitle(tr("Properties"));
    d->m_propertyListWidget->setObjectName(DesignNet::Constants::DESIGN_DOCKNAME_PROPERTY);

    DockWindow *propertyListDock = createDockWidget(d->m_propertyListWidget, Qt::RightDockWidgetArea);
    connect(propertyListDock->toggleViewAction(),
            SIGNAL(triggered(bool)),
            this, SLOT(onDockVisibilityChange(bool)));
    addDelayedMenu();
}

void DesignNetMainWindow::addEditor(IEditor *pEditor)
{
    DesignNetEditor *designNetEditor = qobject_cast<DesignNetEditor*>(pEditor);
    if(designNetEditor)
    {
        DesignView *view = designNetEditor->designNetWidget();
        d->m_stackedWidget->addWidget(view);
        connect(view, SIGNAL(processorSelected(QList<Processor*>)),
                d->m_propertyListWidget, SLOT(processorSelected(QList<Processor*>)));
        connect(view->getSpace(), SIGNAL(processorAdded(Processor*)),
                d->m_propertyListWidget, SLOT(processorAdded(Processor*)));
        d->m_editors.append(pEditor);
    }
}

void DesignNetMainWindow::readSettings()
{
    QSettings *settings = ICore::settings();
    TOTEM_ASSERT(settings, return);

    d->m_dockWidgetStates.clear();

    settings->beginGroup(QLatin1String(dockWindowStates));
    foreach (const QString &key, settings->childKeys())
        d->m_dockWidgetStates.insert(key, settings->value(key));
    settings->endGroup();

    if (d->m_dockWidgetStates.isEmpty())
    {
        d->m_dockWidgetStates = getSettings();
    }
    writeSettings();
}

void DesignNetMainWindow::writeSettings() const
{
    QSettings *settings = ICore::settings();
    settings->beginGroup(QLatin1String(dockWindowStates));
    QHashIterator<QString, QVariant> it(d->m_dockWidgetStates);
    while (it.hasNext())
    {
        it.next();
        settings->setValue(it.key(), it.value());
    }
    settings->endGroup();
}

static bool sortCommands(Command *cmd1, Command *cmd2)
{
    return cmd1->action()->text() < cmd2->action()->text();
}

void DesignNetMainWindow::addDelayedMenu()
{
    qSort(d->m_cmdAboutToAdd.begin(), d->m_cmdAboutToAdd.end(),
          &sortCommands);
    foreach(Command* cmd, d->m_cmdAboutToAdd)
    {
        d->m_viewAction->addAction(cmd);
    }

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

bool DockWindowEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type()){
    case QEvent::Resize:
    case QEvent::ZOrderChange:
        m_window->updateDockWidgetSettings();
        break;
    default:
        break;
    }
    return QObject::eventFilter(obj, event);
}

}
