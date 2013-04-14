#ifndef PLUGINDETAILSVIEW_H
#define PLUGINDETAILSVIEW_H
#include "extensionsystem_global.h"

#include <QWidget>
namespace ExtensionSystem{

class PluginSpec;

namespace Internal {
namespace Ui {
    class PluginDetailsView;
} // namespace Ui
} // namespace Internal

class EXTENSIONSYSTEM_EXPORT PluginDetailsView : public QWidget
{
    Q_OBJECT

public:
    explicit PluginDetailsView(QWidget *parent = 0);
    ~PluginDetailsView();

    void update(PluginSpec *spec);
private:
    Internal::Ui::PluginDetailsView *m_ui;
};

}


#endif // PLUGINDETAILSVIEW_H