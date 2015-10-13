#pragma once
#include "../Common/id.h"


QT_BEGIN_NAMESPACE
class QMenu;
class QMenuBar;
class QAction;
QT_END_NAMESPACE

namespace Core {

class Command;
class Context;
class ActionContainer : public QObject
{
    Q_OBJECT

public:
    enum OnAllDisabledBehavior {
        Disable,
        Hide,
        Show
    };

    virtual void setOnAllDisabledBehavior(OnAllDisabledBehavior behavior) = 0;
    virtual ActionContainer::OnAllDisabledBehavior onAllDisabledBehavior() const = 0;

    virtual Id id() const = 0;

    virtual QMenu *menu() const = 0;
    virtual QMenuBar *menuBar() const = 0;

    virtual QAction *insertLocation(const Id &group) const = 0;
    virtual void appendGroup(const Id &group) = 0;
    virtual void insertGroup(const Id &before, const Id &group) = 0;
    virtual void addAction(Command *action, const Id &group = Id()) = 0;
    virtual void addMenu(ActionContainer *menu, const Id &group = Id()) = 0;
    virtual void addMenu(ActionContainer *before, ActionContainer *menu, const Id &group = Id()) = 0;
    virtual Command *addSeparator(const Context &context, const Id &group = Id(), QAction **outSeparator = 0) = 0;

    virtual void clear() = 0;
};

}
