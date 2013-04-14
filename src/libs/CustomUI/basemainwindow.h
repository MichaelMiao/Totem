#ifndef BASEMAINWINDOW_H
#define BASEMAINWINDOW_H

#include <QMainWindow>
#include <QHash>
#include "customui_global.h"

QT_BEGIN_NAMESPACE
class QDockWidget;
class QMenu;
class QSettings;
QT_END_NAMESPACE

namespace CustomUI{
class DockWindow;
class BaseMainWindowPrivate;
/*!
 * \class BaseMainWindow
 * \brief BaseMainWindow 是主窗口，包含各种停靠窗口，具有记忆功能（保存停靠窗口的位置）
 */
class CUSTOMUI_EXPORT BaseMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit BaseMainWindow(QWidget *parent = 0);
    virtual ~BaseMainWindow();
    /*!
     * \brief addDockForWidget 添加以\a widget 为主界面的停靠窗口
     * \param [in] widget 停靠窗口的主窗口
     * \param [in] area 停靠区域
     * \return DockWindow类型的停靠窗口
     */
    DockWindow *addDockForWidget(QWidget *widget,
                                 Qt::DockWidgetArea area);
    void addDockWidgetDirectly(DockWindow *dockWindow,
                                Qt::DockWidgetArea area);
    QList<DockWindow *> dockWidgets() const;


    //settings
    void saveSettings(QSettings *settings) const;
    void restoreSettings(const QSettings *settings);
    QHash<QString, QVariant> getSettings() const;
    void restoreSettings(const QHash<QString, QVariant> &settings);

signals:
    void resetLayout();
public slots:
    void onDockVisibilityChange(bool bVisible);
    void setDockActionsVisible(bool v);

protected:
    void updateDockWidget(DockWindow *dockWidget);
    void handleVisibilityChanged(bool visible);

    virtual QMenu *createPopupMenu();
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void contextMenuEvent(QContextMenuEvent *event);

    BaseMainWindowPrivate *d;

};
}

#endif // BASEMAINWINDOW_H
