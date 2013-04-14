#ifndef PLUGINVIEW_H
#define PLUGINVIEW_H

#include "extensionsystem_global.h"

#include <QWidget>
#include <QIcon>

QT_BEGIN_NAMESPACE
class QTreeWidgetItem;
QT_END_NAMESPACE

namespace ExtensionSystem{

class PluginManager;
class PluginSpec;
class PluginCollection;

namespace Internal{

namespace Ui {
class PluginView;
}
}//namespace Internal
class EXTENSIONSYSTEM_EXPORT PluginView : public QWidget
{
    Q_OBJECT

public:
    explicit PluginView(PluginManager *pm, QWidget *parent = 0);
    ~PluginView();
    PluginSpec *currentPlugin() const;

signals:
    void currentPluginChanged(ExtensionSystem::PluginSpec *spec);
    void pluginActivated(ExtensionSystem::PluginSpec *spec);
    void pluginSettingsChanged(ExtensionSystem::PluginSpec *spec);

private slots:
    void updatePluginSettings(QTreeWidgetItem *item, int column);
    void updateList();
    void selectPlugin(QTreeWidgetItem *current);
    void activatePlugin(QTreeWidgetItem *item);

private:
    enum ParsedState { ParsedNone = 1,
                       ParsedPartial = 2,
                       ParsedAll = 4,
                       ParsedWithErrors = 8};
    QIcon iconForState(int state);//根据state所指插件状态返回一个QIcon
    int parsePluginSpecs(QTreeWidgetItem *parentItem,
                         Qt::CheckState &groupState,
                         QList<PluginSpec*> plugins);

    void updatePluginDependencies();
    Internal::Ui::PluginView *m_ui;
    PluginManager *m_pManager;
    QStringList m_whitelist;//插件白名单
    QList<QTreeWidgetItem*> m_items;
    QHash<PluginSpec*, QTreeWidgetItem*> m_specToItem;

    QIcon m_okIcon;
    QIcon m_errorIcon;
    QIcon m_notLoadedIcon;

    bool m_allowCheckStateUpdate;
};

}


#endif // PLUGINVIEW_H