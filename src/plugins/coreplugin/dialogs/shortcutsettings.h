#ifndef SHORTCUTSETTINGS_H
#define SHORTCUTSETTINGS_H
#include "actionmanager/commandmappings.h"

#include <QKeySequence>

QT_BEGIN_NAMESPACE
class QKeyEvent;
QT_END_NAMESPACE

namespace Core{

class Command;

namespace Internal {

class ActionManagerPrivate;
class MainWindow;

struct ShortcutItem
{
    Command *m_cmd;
    QKeySequence m_key;
    QTreeWidgetItem *m_item;
};

class ShortcutSettings : public Core::CommandMappings
{
    Q_OBJECT

public:
    ShortcutSettings(QObject *parent = 0);

    QWidget *createPage(QWidget *parent);
    void apply();
    void finish();
    bool matches(const QString &s) const;

protected:
    bool eventFilter(QObject *o, QEvent *e);

private slots:
    void commandChanged(QTreeWidgetItem *current);
    void targetIdentifierChanged();
    void resetTargetIdentifier();
    void removeTargetIdentifier();
    void importAction();
    void exportAction();
    void defaultAction();
    void initialize();

private:
    void setKeySequence(const QKeySequence &key);
    void clear();

    void handleKeyEvent(QKeyEvent *e);
    int translateModifiers(Qt::KeyboardModifiers state, const QString &text);
    using Core::CommandMappings::markPossibleCollisions;
    void markPossibleCollisions(ShortcutItem *);
    void resetCollisionMarker(ShortcutItem *);
    void resetCollisionMarkers();

    QList<ShortcutItem *> m_scitems;
    int m_key[4], m_keyNum;

    QString m_searchKeywords;
    bool m_initialized;
};
}
}

#endif // SHORTCUTSETTINGS_H
