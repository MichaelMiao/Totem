#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include "id.h"
#include "core_global.h"
#include <QObject>

QT_BEGIN_NAMESPACE
class QAction;
QT_END_NAMESPACE

namespace Core {

class IMode;

namespace Internal {
    class MainWindow;
    class ModeTabWidget;
}

class CORE_EXPORT ModeManager : public QObject
{
    Q_OBJECT

public:
    explicit ModeManager(Internal::MainWindow *mainWindow,
                         Internal::ModeTabWidget *modeStack);
    virtual ~ModeManager();

    static void init();
    static ModeManager *instance();

    static IMode *currentMode();
    static IMode *mode(Id id);

    static void addAction(QAction *action, int priority);

    static void activateModeType(Id type);
    static void setModeBarHidden(bool hidden);
    static void activateMode(Id id);
    static void setFocusToCurrentMode();

signals:
    void currentModeAboutToChange(Core::IMode *mode);

    //第一个参数是新的mode，第二个参数（可选）为之前的状态。
    void currentModeChanged(Core::IMode *mode, Core::IMode *oldMode = 0);

private slots:
    void slotActivateMode(int id);
    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);
    void currentTabAboutToChange(int index);
    void currentTabChanged(int index);
    void updateModeToolTip();
    void enabledStateChanged();
};

} // namespace Core

#endif // MODEMANAGER_H
