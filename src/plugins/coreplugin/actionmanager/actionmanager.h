#pragma once
#include "../../coreplugin/actionmanager/command.h"
#include "../../coreplugin/core_global.h"
#include "../Common/id.h"


QT_BEGIN_NAMESPACE
class QAction;
class QShortcut;
class QString;
class QSettings;
QT_END_NAMESPACE

class MainWindow;
namespace Core {

namespace Internal {
class ActionManagerPrivate;
}
class ActionContainer;

class CORE_EXPORT ActionManager : public QObject
{
    Q_OBJECT
public:

    static ActionManager *instance();

    static ActionContainer *createMenu(const Id &id);
    static ActionContainer *createMenuBar(const Id &id);

    static Command *registerAction(QAction *action, const Id &id, const Context &context, bool scriptable = false);
    static Command *registerShortcut(QShortcut *shortcut, const Id &id, const Context &context, bool scriptable = false);

    static Command *command(const Id &id);
    static ActionContainer *actionContainer(const Id &id);

    static QList<Command *> commands();

    static void unregisterAction(QAction *action, const Id &id);
    static void unregisterShortcut(const Id &id);

    static void setPresentationModeEnabled(bool enabled);
    static bool isPresentationModeEnabled();

    static bool hasContext(int context);
	static void setContext(const Context &context);

signals:
    void commandListChanged();
    void commandAdded(const QString &id);

private:
    ActionManager(QObject *parent = 0);
    ~ActionManager();
    Internal::ActionManagerPrivate *d;
};

} // namespace Core
