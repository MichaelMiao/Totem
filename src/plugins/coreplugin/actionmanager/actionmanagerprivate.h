#pragma once
#ifndef ACTIONMANAGERPRIVATE_H
#define ACTIONMANAGERPRIVATE_H

#include <QMap>
#include <QHash>
#include <QMultiHash>
#include <QTimer>
#include "Common/id.h"
#include "commandprivate.h"
#include "actioncontainerprivate.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QSettings;
QT_END_NAMESPACE

class MainWindow;
namespace Core {

namespace Internal {

class ActionContainerPrivate;
class CommandPrivate;

class ActionManagerPrivate : public QObject
{
    Q_OBJECT

public:
    typedef QHash<Core::Id, CommandPrivate *> IdCmdMap;
    typedef QHash<Core::Id, ActionContainerPrivate *> IdContainerMap;

    explicit ActionManagerPrivate();
    ~ActionManagerPrivate();

    void initialize();

    void setContext(const Context &context);
    bool hasContext(int context) const;

    void saveSettings(QSettings *settings);

    void showShortcutPopup(const QString &shortcut);
    bool hasContext(const Context &context) const;
    Action *overridableAction(const Id &id);

public slots:
    void containerDestroyed();

    void actionTriggered();
    void shortcutTriggered();

public:

    IdCmdMap m_idCmdMap;

    IdContainerMap m_idContainerMap;

	Context m_context;

    QLabel *m_presentationLabel;
    QTimer m_presentationLabelTimer;
};

} // namespace Internal
} // namespace Core

#endif // ACTIONMANAGERPRIVATE_H
