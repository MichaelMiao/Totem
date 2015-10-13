#include "stdafx.h"
#include "actionmanager.h"
#include <coreplugin/icore.h>
#include <utils/totemassert.h>
#include <QAction>
#include <QDebug>
#include <QDesktopWidget>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QShortcut>
#include "../mainwindow/mainwindow.h"
#include "extensionsystem/pluginmanager.h"
#include "actioncontainerprivate.h"
#include "actionmanagerprivate.h"


namespace {
    enum { warnAboutFindFailures = 0 };
}

using namespace Core;
using namespace Core::Internal;

static ActionManager *m_instance = 0;
/*!
    \internal
*/
ActionManager::ActionManager(QObject *parent)
    : QObject(parent),
      d(new ActionManagerPrivate())
{
    m_instance = this;
}

/*!
    \internal
*/
ActionManager::~ActionManager()
{
    delete d;
}

/*!
 * \return Singleton action manager instance
 */
ActionManager *ActionManager::instance()
{
	static ActionManager mgr;
    return &mgr;
}

/*!
    \brief Creates a new menu with the given \a id.

    Returns a new ActionContainer that you can use to get the QMenu instance
    or to add menu items to the menu. The ActionManager owns
    the returned ActionContainer.
    Add your menu to some other menu or a menu bar via the
    ActionManager::actionContainer and ActionContainer::addMenu methods.
*/
ActionContainer *ActionManager::createMenu(const Id &id)
{
    const ActionManagerPrivate::IdContainerMap::const_iterator it = m_instance->d->m_idContainerMap.constFind(id);
    if (it !=  m_instance->d->m_idContainerMap.constEnd())
        return it.value();

    QMenu *m = new QMenu(ICore::instance()->getFirstMainWindow());
    m->setObjectName(QLatin1String(id.name()));

    MenuActionContainer *mc = new MenuActionContainer(id);
    mc->setMenu(m);

    m_instance->d->m_idContainerMap.insert(id, mc);
    connect(mc, SIGNAL(destroyed()), m_instance->d, SLOT(containerDestroyed()));

    return mc;
}

/*!
    \brief Creates a new menu bar with the given \a id.

    Returns a new ActionContainer that you can use to get the QMenuBar instance
    or to add menus to the menu bar. The ActionManager owns
    the returned ActionContainer.
*/
ActionContainer *ActionManager::createMenuBar(const Id &id)
{
    const ActionManagerPrivate::IdContainerMap::const_iterator it = m_instance->d->m_idContainerMap.constFind(id);
	if (it != instance()->d->m_idContainerMap.constEnd())
        return it.value();

    QMenuBar *mb = new QMenuBar; // No parent (System menu bar on Mac OS X)
    mb->setObjectName(id.toString());

    MenuBarActionContainer *mbc = new MenuBarActionContainer(id);
    mbc->setMenuBar(mb);

    m_instance->d->m_idContainerMap.insert(id, mbc);
    connect(mbc, SIGNAL(destroyed()), m_instance->d, SLOT(containerDestroyed()));

    return mbc;
}

/*!
    \brief Makes an \a action known to the system under the specified \a id.

    Returns a command object that represents the action in the application and is
    owned by the ActionManager. You can register several actions with the
    same \a id as long as the \a context is different. In this case
    a trigger of the actual action is forwarded to the registered QAction
    for the currently active context.
    A scriptable action can be called from a script without the need for the user
    to interact with it.
*/
Command *ActionManager::registerAction(QAction *action, const Id &id, const Context &context, bool scriptable)
{
    Action *a = instance()->d->overridableAction(id);
    if (a) {
        a->addOverrideAction(action, context, scriptable);
		emit instance()->commandListChanged();
		emit instance()->commandAdded(id.toString());
    }
    return a;
}

/*!
    \brief Makes a \a shortcut known to the system under the specified \a id.

    Returns a command object that represents the shortcut in the application and is
    owned by the ActionManager. You can registered several shortcuts with the
    same \a id as long as the \a context is different. In this case
    a trigger of the actual shortcut is forwarded to the registered QShortcut
    for the currently active context.
    A scriptable shortcut can be called from a script without the need for the user
    to interact with it.
*/
Command *ActionManager::registerShortcut(QShortcut *shortcut, const Id &id, const Context &context, bool scriptable)
{
    Shortcut *sc = 0;
    if (CommandPrivate *c = m_instance->d->m_idCmdMap.value(id, 0)) {
        sc = qobject_cast<Shortcut *>(c);
        if (!sc) {
            qWarning() << "registerShortcut: id" << id.name()
                       << "is registered with a different command type.";
            return c;
        }
    } else {
        sc = new Shortcut(id);
        m_instance->d->m_idCmdMap.insert(id, sc);
    }

    if (sc->shortcut()) {
        qWarning() << "registerShortcut: action already registered, id" << id.name() << ".";
        return sc;
    }

    if (!m_instance->d->hasContext(context))
        shortcut->setEnabled(false);
    shortcut->setObjectName(id.toString());
    shortcut->setParent(ICore::instance()->getFirstMainWindow());
    sc->setShortcut(shortcut);
    sc->setScriptable(scriptable);

    if (context.isEmpty())
        sc->setContext(Context(0));
    else
        sc->setContext(context);

    emit m_instance->commandListChanged();
    emit m_instance->commandAdded(id.toString());

    if (isPresentationModeEnabled())
        connect(sc->shortcut(), SIGNAL(activated()), m_instance->d, SLOT(shortcutTriggered()));
    return sc;
}

/*!
    \brief Returns the Command object that is known to the system
    under the given \a id.

    \sa ActionManager::registerAction()
*/
Command *ActionManager::command(const Id &id)
{
    const ActionManagerPrivate::IdCmdMap::const_iterator it = m_instance->d->m_idCmdMap.constFind(id);
    if (it == m_instance->d->m_idCmdMap.constEnd()) {
        if (warnAboutFindFailures)
            qWarning() << "ActionManagerPrivate::command(): failed to find :"
                       << id.name();
        return 0;
    }
    return it.value();
}

/*!
    \brief Returns the IActionContainter object that is know to the system
    under the given \a id.

    \sa ActionManager::createMenu()
    \sa ActionManager::createMenuBar()
*/
ActionContainer *ActionManager::actionContainer(const Id &id)
{
    const ActionManagerPrivate::IdContainerMap::const_iterator it = m_instance->d->m_idContainerMap.constFind(id);
    if (it == m_instance->d->m_idContainerMap.constEnd()) {
        if (warnAboutFindFailures)
            qWarning() << "ActionManagerPrivate::actionContainer(): failed to find :"
                       << id.name();
        return 0;
    }
    return it.value();
}

/*!
 * \brief Returns all commands that have been registered.
 */
QList<Command *> ActionManager::commands()
{
    // transform list of CommandPrivate into list of Command
    QList<Command *> result;
    foreach (Command *cmd, m_instance->d->m_idCmdMap.values())
        result << cmd;
    return result;
}

/*!
    \brief Removes the knowledge about an \a action under the specified \a id.

    Usually you do not need to unregister actions. The only valid use case for unregistering
    actions, is for actions that represent user definable actions, like for the custom Locator
    filters. If the user removes such an action, it also has to be unregistered from the action manager,
    to make it disappear from shortcut settings etc.
*/
void ActionManager::unregisterAction(QAction *action, const Id &id)
{
    Action *a = 0;
    CommandPrivate *c = m_instance->d->m_idCmdMap.value(id, 0);
    TOTEM_ASSERT(c, return);
    a = qobject_cast<Action *>(c);
    if (!a) {
        qWarning() << "unregisterAction: id" << id.name()
                   << "is registered with a different command type.";
        return;
    }
    a->removeOverrideAction(action);
    if (a->isEmpty()) {
        // clean up
        // ActionContainers listen to the commands' destroyed signals
        ICore::instance()->getFirstMainWindow()->removeAction(a->action());
        delete a->action();
        m_instance->d->m_idCmdMap.remove(id);
        delete a;
    }
    emit m_instance->commandListChanged();
}

/*!
    \brief Removes the knowledge about a shortcut under the specified \a id.

    Usually you do not need to unregister shortcuts. The only valid use case for unregistering
    shortcuts, is for shortcuts that represent user definable actions. If the user removes such an action,
    a corresponding shortcut also has to be unregistered from the action manager,
    to make it disappear from shortcut settings etc.
*/
void ActionManager::unregisterShortcut(const Core::Id &id)
{
    Shortcut *sc = 0;
    CommandPrivate *c = m_instance->d->m_idCmdMap.value(id, 0);
    TOTEM_ASSERT(c, return);
    sc = qobject_cast<Shortcut *>(c);
    if (!sc) {
        qWarning() << "unregisterShortcut: id" << id.name()
                   << "is registered with a different command type.";
        return;
    }
    delete sc->shortcut();
    m_instance->d->m_idCmdMap.remove(id);
    delete sc;
    emit m_instance->commandListChanged();
}


void ActionManager::setPresentationModeEnabled(bool enabled)
{
    if (enabled == isPresentationModeEnabled())
        return;

    // Signal/slots to commands:
    foreach (Command *c, commands()) {
        if (c->action()) {
            if (enabled)
                connect(c->action(), SIGNAL(triggered()), m_instance->d, SLOT(actionTriggered()));
            else
                disconnect(c->action(), SIGNAL(triggered()), m_instance->d, SLOT(actionTriggered()));
        }
        if (c->shortcut()) {
            if (enabled)
                connect(c->shortcut(), SIGNAL(activated()), m_instance->d, SLOT(shortcutTriggered()));
            else
                disconnect(c->shortcut(), SIGNAL(activated()), m_instance->d, SLOT(shortcutTriggered()));
        }
    }

    // The label for the shortcuts:
    if (!m_instance->d->m_presentationLabel) {
        m_instance->d->m_presentationLabel = new QLabel(0, Qt::ToolTip | Qt::WindowStaysOnTopHint);
        QFont font = m_instance->d->m_presentationLabel->font();
        font.setPixelSize(45);
        m_instance->d->m_presentationLabel->setFont(font);
        m_instance->d->m_presentationLabel->setAlignment(Qt::AlignCenter);
        m_instance->d->m_presentationLabel->setMargin(5);

        connect(&m_instance->d->m_presentationLabelTimer, SIGNAL(timeout()), m_instance->d->m_presentationLabel, SLOT(hide()));
    } else {
        m_instance->d->m_presentationLabelTimer.stop();
        delete m_instance->d->m_presentationLabel;
        m_instance->d->m_presentationLabel = 0;
    }
}

bool ActionManager::isPresentationModeEnabled()
{
    return m_instance->d->m_presentationLabel;
}

bool ActionManager::hasContext(int context)
{
    return m_instance->d->m_context.contains(context);
}

void Core::ActionManager::setContext(const Context &context)
{
	if (instance()->d)
		instance()->d->setContext(context);
}

ActionManagerPrivate::ActionManagerPrivate()
  : m_presentationLabel(0)
{
    m_presentationLabelTimer.setInterval(1000);
}

ActionManagerPrivate::~ActionManagerPrivate()
{
    // first delete containers to avoid them reacting to command deletion
    foreach (ActionContainerPrivate *container, m_idContainerMap)
        disconnect(container, SIGNAL(destroyed()), this, SLOT(containerDestroyed()));
    qDeleteAll(m_idContainerMap.values());
    qDeleteAll(m_idCmdMap.values());
}

QDebug operator<<(QDebug in, const Context &context)
{
    in << "CONTEXT: ";
    foreach (int c, context)
        in << "   " << c << Id::fromUniqueIdentifier(c).toString();
    return in;
}

void ActionManagerPrivate::setContext(const Context &context)
{
    // here are possibilities for speed optimization if necessary:
    // let commands (de-)register themselves for contexts
    // and only update commands that are either in old or new contexts
    m_context = context;
    const IdCmdMap::const_iterator cmdcend = m_idCmdMap.constEnd();
    for (IdCmdMap::const_iterator it = m_idCmdMap.constBegin(); it != cmdcend; ++it)
        it.value()->setCurrentContext(m_context);
}

bool ActionManagerPrivate::hasContext(const Context &context) const
{
    for (int i = 0; i < m_context.size(); ++i) {
        if (context.contains(m_context.at(i)))
            return true;
    }
    return false;
}

void ActionManagerPrivate::containerDestroyed()
{
    ActionContainerPrivate *container = static_cast<ActionContainerPrivate *>(sender());
    m_idContainerMap.remove(m_idContainerMap.key(container));
}

void ActionManagerPrivate::actionTriggered()
{
    QAction *action = qobject_cast<QAction *>(QObject::sender());
    if (action)
        showShortcutPopup(action->shortcut().toString());
}

void ActionManagerPrivate::shortcutTriggered()
{
    QShortcut *sc = qobject_cast<QShortcut *>(QObject::sender());
    if (sc)
        showShortcutPopup(sc->key().toString());
}

void ActionManagerPrivate::showShortcutPopup(const QString &shortcut)
{
    if (shortcut.isEmpty() || !ActionManager::isPresentationModeEnabled())
        return;

    m_presentationLabel->setText(shortcut);
    m_presentationLabel->adjustSize();

	MainWindow* pMainWindow = ICore::instance()->getFirstMainWindow();
    if (pMainWindow)
    {
		QPoint p = pMainWindow->mapToGlobal(pMainWindow->rect().center() - m_presentationLabel->rect().center());
		m_presentationLabel->move(p);

		m_presentationLabel->show();
		m_presentationLabel->raise();
		m_presentationLabelTimer.start();
    }
}

Action *ActionManagerPrivate::overridableAction(const Id &id)
{
    Action *a = 0;
    if (CommandPrivate *c = m_idCmdMap.value(id, 0)) {
        a = qobject_cast<Action *>(c);
        if (!a) {
            qWarning() << "registerAction: id" << id.name()
                       << "is registered with a different command type.";
            return 0;
        }
    } else {
        a = new Action(id);
        m_idCmdMap.insert(id, a);
        ICore::instance()->getFirstMainWindow()->addAction(a->action());
        a->action()->setObjectName(id.toString());
        a->action()->setShortcutContext(Qt::ApplicationShortcut);
        a->setCurrentContext(m_context);

        if (ActionManager::isPresentationModeEnabled())
            connect(a->action(), SIGNAL(triggered()), this, SLOT(actionTriggered()));
    }

    return a;
}

static const char settingsGroup[] = "KeyBindings";
static const char idKey[] = "ID";
static const char sequenceKey[] = "Keysequence";

void ActionManagerPrivate::initialize()
{
	QSettings *settings = ExtensionSystem::PluginManager::instance()->settings();
    const int shortcuts = settings->beginReadArray(QLatin1String(settingsGroup));
    for (int i = 0; i < shortcuts; ++i) {
        settings->setArrayIndex(i);
        const QKeySequence key(settings->value(QLatin1String(sequenceKey)).toString());
        const Id id = Id(settings->value(QLatin1String(idKey)).toString());

        Command *cmd = ActionManager::command(id);
        if (cmd)
            cmd->setKeySequence(key);
    }
    settings->endArray();
}

void ActionManagerPrivate::saveSettings(QSettings *settings)
{
    settings->beginWriteArray(QLatin1String(settingsGroup));
    int count = 0;

    const IdCmdMap::const_iterator cmdcend = m_idCmdMap.constEnd();
    for (IdCmdMap::const_iterator j = m_idCmdMap.constBegin(); j != cmdcend; ++j) {
        const Id id = j.key();
        CommandPrivate *cmd = j.value();
        QKeySequence key = cmd->keySequence();
        if (key != cmd->defaultKeySequence()) {
            settings->setArrayIndex(count);
            settings->setValue(QLatin1String(idKey), id.toString());
            settings->setValue(QLatin1String(sequenceKey), key.toString());
            count++;
        }
    }

    settings->endArray();
}
