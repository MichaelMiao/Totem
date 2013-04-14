#ifndef DOCKWINDOW_H
#define DOCKWINDOW_H

#include "customui_global.h"
#include <QDockWidget>
#include <QToolButton>

namespace CustomUI{

namespace Internal{

class DockWindowTitlePrivate;

class DockWindowTitle : public QWidget
{
    Q_OBJECT
public:
    enum SystemButton{
        DOCKBUTTON_CLOSE = 0x01,
        DOCKBUTTON_MINMAX = 0x02
    };
    Q_DECLARE_FLAGS(SystemButtons, SystemButton)
    explicit DockWindowTitle(QDockWidget *parent,
                             SystemButtons btns);
    void addSystemButton(QToolButton *button);
    void setTitle(const QString &str);
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void enterEvent(QEvent *e);
    void leaveEvent(QEvent *e);

    virtual QSize sizeHint()const;
    virtual QSize minimumSizeHint() const;
    void updateActions();

public slots:
    virtual void onClose();
    virtual void onMinMaxClicked();
signals:
    void closeClicked();
    void minMaxClicked(bool &bMin);
private:
    DockWindowTitlePrivate *d;
    QDockWidget *m_parent;
    bool m_bHover;
    bool m_bMin;
};

}//namespace Internal

class CUSTOMUI_EXPORT DockWindow : public QDockWidget
{
    Q_OBJECT
public:
    explicit DockWindow(const QString &title, QWidget *parent = 0);
    virtual ~DockWindow();

public slots:
    virtual void onClosed();
    virtual void onMinMax(bool &bMin);
    virtual void onTopLevelChanged(bool topLevel);
signals:
    void closeClicked();
    void minMaxClicked(bool &bMin);
public slots:
private:
    QWidget *m_widget;
    Internal::DockWindowTitle *m_titleBar;
};
}//namespace CustomUI
Q_DECLARE_OPERATORS_FOR_FLAGS(CustomUI::Internal::DockWindowTitle::SystemButtons)

#endif // DOCKWINDOW_H
