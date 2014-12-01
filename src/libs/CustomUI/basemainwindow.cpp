#include "basemainwindow.h"
#include "dockwindow.h"
#include "utils/totemassert.h"

#include <QDockWidget>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QSettings>
const char dockWidgetVisual[]   = "dockWidgetVisual";
const char managedDockWidget[]  = "managedDockWidget";
const char KeyStateOfWindow[]        = "windowState";
static const int settingsVersion = 1;

namespace CustomUI{

class BaseMainWindowPrivate
{
public:
    BaseMainWindowPrivate();

    QAction m_separator;    //!< 分隔条
    QAction m_resetAction;  //!< 重置布局

    bool m_handleDockVisible;
	bool m_handleDockVisibilityChanges;
};
BaseMainWindowPrivate::BaseMainWindowPrivate()
    : m_separator(0),
      m_resetAction(BaseMainWindow::tr("Reset Layout"), 0),
      m_handleDockVisible(true)

{
    m_separator.setSeparator(true);
}

//---------------------------------------------------
BaseMainWindow::BaseMainWindow(QWidget *parent) :
    QMainWindow(parent),
    d(new BaseMainWindowPrivate)
{
    connect(&d->m_resetAction, SIGNAL(triggered()),
            this, SIGNAL(resetLayout()));
}

BaseMainWindow::~BaseMainWindow()
{
    delete d;
}

DockWindow *BaseMainWindow::addDockForWidget(QWidget *widget,
                                             Qt::DockWidgetArea area)
{
    DockWindow *dockWindow = new DockWindow(widget->windowTitle(), this);
    dockWindow->setWidget(widget);
    // objectName将会用在QSetting当中
    const QString objectName = widget->objectName();
    dockWindow->setObjectName(objectName);
    this->addDockWidgetDirectly(dockWindow, area);
    return dockWindow;
}
/*!
 * \brief BaseMainWindow::addDockWidgetDirectly
 * \param dockWindow
 * \param area
 * 这里的dock需要是已经有名字的
 */
void BaseMainWindow::addDockWidgetDirectly(DockWindow *dockWindow,
                                           Qt::DockWidgetArea area)
{
    connect(dockWindow, SIGNAL(visibilityChanged(bool)),
            this, SLOT(onDockVisibilityChange(bool)));

    dockWindow->setProperty(dockWidgetVisual, true);
    addDockWidget(area, dockWindow);
}

QList<DockWindow *> BaseMainWindow::dockWidgets() const
{
	return this->findChildren<DockWindow *>();
}

void BaseMainWindow::saveSettings(QSettings *settings) const
{
    QHash<QString, QVariant> hash = getSettings();
    QHashIterator<QString, QVariant> it(hash);
    while (it.hasNext())
    {
        it.next();
        settings->setValue(it.key(), it.value());
    }
}

void BaseMainWindow::onDockVisibilityChange(bool bVisible)
{
    if (d->m_handleDockVisible)
        sender()->setProperty(dockWidgetVisual, bVisible);
}

void BaseMainWindow::setDockActionsVisible(bool v)
{
    foreach (const QDockWidget *dockWidget, dockWidgets())
        dockWidget->toggleViewAction()->setVisible(v);
    d->m_resetAction.setVisible(v);
}

void BaseMainWindow::handleVisibilityChanged(bool visible)
{
    d->m_handleDockVisible = false;
    foreach (DockWindow *dockWidget, dockWidgets())
    {
        if (dockWidget->isFloating())
        {
            dockWidget->setVisible(visible
                && dockWidget->property(dockWidgetVisual).toBool());
        }
    }
    if (visible)
        d->m_handleDockVisible = true;
}



void BaseMainWindow::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    handleVisibilityChanged(false);
}

void BaseMainWindow::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    handleVisibilityChanged(true);
}

static bool actionLessThan(const QAction *action1, const QAction *action2)
{
    TOTEM_ASSERT(action1, return true);
    TOTEM_ASSERT(action2, return false);
    return action1->text().toLower() < action2->text().toLower();
}

void BaseMainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createPopupMenu();
    menu->exec(event->globalPos());
    delete menu;
}

QMenu *BaseMainWindow::createPopupMenu()
{
    QList<QAction *> actions;
    QList<DockWindow *> dockwidgets = this->findChildren<DockWindow *>();
    for (int i = 0; i < dockwidgets.size(); ++i)
    {
        DockWindow *dockWidget = dockwidgets.at(i);
        if (dockWidget->property(managedDockWidget).isNull()
                && dockWidget->parentWidget() == this) {
            actions.append(dockwidgets.at(i)->toggleViewAction());
        }
    }
    qSort(actions.begin(), actions.end(), actionLessThan);
    QMenu *menu = new QMenu(this);
    foreach (QAction *action, actions)
        menu->addAction(action);

    menu->addAction(&d->m_separator);
    menu->addAction(&d->m_resetAction);
    return menu;
}

QHash<QString, QVariant> BaseMainWindow::getSettings() const
{
    QHash<QString, QVariant> settings;
    settings.insert(QLatin1String(KeyStateOfWindow), saveState(settingsVersion));
    foreach (QDockWidget *dockWidget, dockWidgets())
    {
        settings.insert(dockWidget->objectName(),
                dockWidget->property(dockWidgetVisual));
    }
    return settings;
}
/*!
 * \brief BaseMainWindow::restoreSettings
 * \param settings
 *  从QSetting中恢复状态
 */
void BaseMainWindow::restoreSettings(const QSettings *settings)
{
    QHash<QString, QVariant> hash;
    foreach(QString key, settings->childKeys())
    {
        hash.insert(key, settings->value(key));
    }
    restoreSettings(hash);
}

/*!
 * \brief BaseMainWindow::restoreSettings
 * \param settings
 *  根据setttings中的数据恢复mainwindow的状态
 */
void BaseMainWindow::restoreSettings(const QHash<QString, QVariant> &settings)
{
    QByteArray data = settings.value(KeyStateOfWindow, QByteArray()).toByteArray();
    if(!data.isEmpty())
        restoreState(data, settingsVersion);
    foreach(DockWindow *dock, dockWidgets())
    {
        dock->setProperty(dockWidgetVisual,
             settings.value(dock->objectName(), false));
    }
}

void BaseMainWindow::setTrackingEnabled( bool enabled )
{
	if (enabled) {
		d->m_handleDockVisible = true;
		foreach (QDockWidget *dockWidget, dockWidgets())
			dockWidget->setProperty(dockWidgetVisual, dockWidget->isVisible());
	} else {
		d->m_handleDockVisible = false;
	}
}

}
