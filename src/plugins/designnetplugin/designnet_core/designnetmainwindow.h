#ifndef DESIGNNETMAINWINDOW_H
#define DESIGNNETMAINWINDOW_H

#include "libs/CustomUI/basemainwindow.h"
namespace Core{
class IMode;
class IEditor;
}
namespace CustomUI{
class DockWindow;
}
namespace DesignNet{
class DesignNetMainWindow;
class DesignNetTool;
class DesignNetMainWindowPrivate;
class DockWindowEventFilter : public QObject
{
    Q_OBJECT
public:
    DockWindowEventFilter(DesignNetMainWindow *w)
        : m_window(w)
    {

    }
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    DesignNetMainWindow *m_window;
};
class DesignNetMainWindow : public CustomUI::BaseMainWindow
{
    Q_OBJECT
    friend class DesignNetFormManager;
public:
    explicit DesignNetMainWindow(QWidget *parent = 0);
    ~DesignNetMainWindow();

    static DesignNetMainWindow *instance();
    void addEditor(Core::IEditor *pEditor);
    void extensionsInitialized();
    void onModeChanged(Core::IMode *mode);




    CustomUI::DockWindow *createDockWidget(QWidget *widget,
                          Qt::DockWidgetArea area = Qt::LeftDockWidgetArea);
    CustomUI::DockWindow *dockWindow(const QString &objectName);
    void setVisibleEditor(Core::IEditor *xmlEditor);
protected:

    void readSettings();
    void writeSettings() const;
    void addDelayedMenu();
signals:

public slots:
    void updateDockWidgetSettings();
    void onResetLayout();
private:
    DesignNetMainWindowPrivate *d;
    friend class DockWindowEventFilter;
};
}

#endif // DESIGNNETMAINWINDOW_H
