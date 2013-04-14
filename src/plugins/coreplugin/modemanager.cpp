#include "modemanager.h"
#include "icontext.h"
#include "command.h"
#include "mainwindow.h"
#include "imode.h"
#include "extensionsystem/pluginmanager.h"
#include "modewidget.h"
#include "utils/totemassert.h"
#include "coreconstants.h"
#include "actionmanager/actionmanager.h"

#include <QMap>
#include <QVector>
#include <QSignalMapper>
#include <QAction>
#include <QShortcut>
using namespace Core::Internal;
namespace Core{

struct ModeManagerPrivate
{
    Internal::MainWindow *m_mainWindow;
    Internal::ModeTabWidget *m_modeStack;

    QMap<QAction*, int> m_actions;
    QVector<IMode*> m_modes;
    QVector<Command*> m_modeShortcuts;
    QSignalMapper *m_signalMapper;
    Context m_addedContexts;
    int m_oldCurrent;
};

static ModeManagerPrivate *d = 0;
static ModeManager *m_instance = 0;

static int indexOf(Id id)
{
    for (int i = 0; i < d->m_modes.count(); ++i) {
        if (d->m_modes.at(i)->id() == id)
            return i;
    }
    qDebug() << "Warning, no such mode:" << id.toString();
    return -1;
}

ModeManager::ModeManager(Internal::MainWindow *mainWindow,
                         Internal::ModeTabWidget *modeStack)
{
    m_instance = this;
    d = new ModeManagerPrivate();
    d->m_mainWindow = mainWindow;
    d->m_modeStack = modeStack;
    d->m_signalMapper = new QSignalMapper(this);
    d->m_oldCurrent = -1;

    connect(d->m_modeStack, SIGNAL(currentAboutToShow(int)), SLOT(currentTabAboutToChange(int)));
    connect(d->m_modeStack, SIGNAL(currentChanged(int)), SLOT(currentTabChanged(int)));
    connect(d->m_signalMapper, SIGNAL(mapped(int)), this, SLOT(slotActivateMode(int)));

}

ModeManager::~ModeManager()
{
    if (d)
    {
        delete d;
        d = 0;
    }

    m_instance = 0;
}

void ModeManager::init()
{
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*)),
                     m_instance, SLOT(objectAdded(QObject*)));
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(aboutToRemoveObject(QObject*)),
                     m_instance, SLOT(aboutToRemoveObject(QObject*)));

}
ModeManager *ModeManager::instance()
{
    return m_instance;
}

IMode *ModeManager::currentMode()
{
    int currentIndex = d->m_modeStack->currentIndex();
    if (currentIndex < 0)
        return 0;
    return d->m_modes.at(currentIndex);
}

IMode *ModeManager::mode(Id id)
{
    const int index = indexOf(id);
    if (index >= 0)
        return d->m_modes.at(index);
    return 0;
}

void ModeManager::addAction(QAction *action, int priority)
{
    d->m_actions.insert(action, priority);

    // 计算优先级比priority大的action的个数
    int index = 0;
    foreach (int p, d->m_actions)
    {
        if (p > priority)
            ++index;
    }

//    d->m_actionBar->insertAction(index, action);
}

void ModeManager::activateModeType(Id type)
{
    if (currentMode() && currentMode()->type() == type)
        return;
    int index = -1;
    for (int i = 0; i < d->m_modes.count(); ++i)
    {
        if (d->m_modes.at(i)->type() == type)
        {
            index = i;
            break;
        }
    }
    if (index != -1)
        d->m_modeStack->setCurrentIndex(index);
}

void ModeManager::setModeBarHidden(bool hidden)
{
    d->m_modeStack->setSelectionWidgetHidden(hidden);
}

void ModeManager::activateMode(Id id)
{
    const int index = indexOf(id);
    if (index >= 0)
        d->m_modeStack->setCurrentIndex(index);
}

void ModeManager::setFocusToCurrentMode()
{
    IMode *mode = currentMode();
    TOTEM_ASSERT(mode, return);
    QWidget *widget = mode->widget();
    if (widget) {
        QWidget *focusWidget = widget->focusWidget();
        if (focusWidget)
            focusWidget->setFocus();
        else
            widget->setFocus();
    }
}

void ModeManager::slotActivateMode(int id)
{
    m_instance->activateMode(Id::fromUniqueIdentifier(id));
}

void ModeManager::objectAdded(QObject *obj)
{
    IMode *mode = Aggregation::query<IMode>(obj);
    if (!mode)
        return;

    d->m_mainWindow->addContextObject(mode);

    // 得到obj的index
    int index = 0;
    foreach (const IMode *m, d->m_modes)
        if (m->priority() > mode->priority())
            ++index;

    d->m_modes.insert(index, mode);
    d->m_modeStack->insertTab(index, mode->widget(), mode->icon(), mode->displayName());
    d->m_modeStack->setTabEnabled(index, mode->isEnabled());

    // 注册快捷键
    const Id shortcutId(QLatin1String("Totem.Mode.") + mode->id().toString());
    QShortcut *shortcut = new QShortcut(d->m_mainWindow);
    shortcut->setWhatsThis(tr("%1").arg(mode->displayName()));
    Command *cmd = ActionManager::registerShortcut(shortcut, shortcutId, Context(Constants::C_GLOBAL));

    d->m_modeShortcuts.insert(index, cmd);
    connect(cmd, SIGNAL(keySequenceChanged()), this, SLOT(updateModeToolTip()));
    for (int i = 0; i < d->m_modeShortcuts.size(); ++i)
    {
        Command *currentCmd = d->m_modeShortcuts.at(i);
        // we need this hack with currentlyHasDefaultSequence
        // because we call setDefaultShortcut multiple times on the same cmd
        // and still expect the current shortcut to change with it
        bool currentlyHasDefaultSequence = (currentCmd->keySequence()
                                            == currentCmd->defaultKeySequence());
        currentCmd->setDefaultKeySequence(QKeySequence(QString::fromLatin1("Ctrl+%1").arg(i+1)));
        if (currentlyHasDefaultSequence)
            currentCmd->setKeySequence(currentCmd->defaultKeySequence());
    }

    d->m_signalMapper->setMapping(shortcut, mode->id().uniqueIdentifier());
    connect(shortcut, SIGNAL(activated()), d->m_signalMapper, SLOT(map()));
    connect(mode, SIGNAL(enabledStateChanged(bool)),
            this, SLOT(enabledStateChanged()));
}

void ModeManager::aboutToRemoveObject(QObject *obj)
{
}

void ModeManager::currentTabAboutToChange(int index)
{
    if (index >= 0)
    {
        IMode *mode = d->m_modes.at(index);
        if (mode)
            emit currentModeAboutToChange(mode);
    }
}

void ModeManager::currentTabChanged(int index)
{
    // Tab index changes to -1 when there is no tab left.
    if (index >= 0)
    {
        IMode *mode = d->m_modes.at(index);

        // FIXME: This hardcoded context update is required for the Debug and Edit modes, since
        // they use the editor widget, which is already a context widget so the main window won't
        // go further up the parent tree to find the mode context.
        ICore::updateAdditionalContexts(d->m_addedContexts, mode->context());
        d->m_addedContexts = mode->context();

        IMode *oldMode = 0;
        if (d->m_oldCurrent >= 0)
            oldMode = d->m_modes.at(d->m_oldCurrent);
        d->m_oldCurrent = index;
        emit currentModeChanged(mode, oldMode);
    }
}

void ModeManager::updateModeToolTip()
{
    Command *cmd = qobject_cast<Command *>(sender());
    if (cmd)
    {
        int index = d->m_modeShortcuts.indexOf(cmd);
        if (index != -1)
            d->m_modeStack->setTabToolTip(index, cmd->stringWithAppendedShortcut(cmd->shortcut()->whatsThis()));
    }
}

void ModeManager::enabledStateChanged()
{
    IMode *mode = qobject_cast<IMode *>(sender());
    TOTEM_ASSERT(mode, return);
    int index = d->m_modes.indexOf(mode);
    TOTEM_ASSERT(index >= 0, return);
    d->m_modeStack->setTabEnabled(index, mode->isEnabled());

    if (mode == currentMode() && !mode->isEnabled())
    {
        for (int i = 0; i < d->m_modes.count(); ++i)
        {
            if (d->m_modes.at(i) != mode &&
                d->m_modes.at(i)->isEnabled())
            {
                activateMode(d->m_modes.at(i)->id());
                break;
            }
        }
    }
}

}


