#ifndef IOUTPUTPANE_H
#define IOUTPUTPANE_H

#include "core_global.h"

#include <QObject>
#include <QList>
#include <QString>

QT_BEGIN_NAMESPACE
class QWidget;
QT_END_NAMESPACE

namespace Core{
class CORE_EXPORT IOutputPane : public QObject
{
    Q_OBJECT

public:
    IOutputPane(QObject *parent = 0) : QObject(parent) {}

    virtual QWidget *outputWidget(QWidget *parent) = 0;
    virtual QList<QWidget *> toolBarWidgets() const = 0;
    virtual QString displayName() const = 0;

    // -1 表示不显示在状态栏
    // 100...0 显示从前到后
    virtual int priorityInStatusBar() const = 0;

    virtual void clearContents() = 0;
    virtual void visibilityChanged(bool visible) = 0;


    virtual void setFocus() = 0;

    virtual bool hasFocus() const = 0;

    virtual bool canFocus() const = 0;

    virtual bool canNavigate() const = 0;
    virtual bool canNext() const = 0;
    virtual bool canPrevious() const = 0;
    virtual void goToNext() = 0;
    virtual void goToPrev() = 0;

public slots:
    void popup() { popup(true, false); }
    void popup(bool withFocus) { popup(withFocus, false); }
    void popup(bool withFocus, bool ensureSizeHint) { emit showPage(withFocus, ensureSizeHint); }
    void hide() { emit hidePage(); }
    void toggle() { toggle(true); }
    void toggle(bool withFocusIfShown) { emit togglePage(withFocusIfShown); }
    void navigateStateChanged() { emit navigateStateUpdate(); }

signals:
    void showPage(bool withFocus, bool ensureSizeHint);
    void hidePage();
    void togglePage(bool withFocusIfShown);
    void navigateStateUpdate();
};
}


#endif // IOUTPUTPANE_H
