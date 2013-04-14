#ifndef UIPOPUPWIDGET_P_H
#define UIPOPUPWIDGET_P_H

#include "basepopupwidget_p.h"
#include "uipopupwidget.h"

namespace CustomUI{
class uiPopupWidget;
class uiPopupWidgetPrivate : public BasePopupWidgetPrivate
{
public:
    uiPopupWidgetPrivate(uiPopupWidget *pObj);
    virtual ~uiPopupWidgetPrivate();
    virtual void init();
    virtual QWidget* mouseOver();
    virtual bool eventFilter(QObject *obj, QEvent *event);
    void temporarilyHiddenOn();
    void temporarilyHiddenOff();
public slots:
    void updateVisibility();
    void onApplicationDeactive();
protected:
    bool    m_bActive;
    bool    m_bAutoShow;
    int     m_iShowDelay;
    bool    m_bAutoHide;
    int     m_iHideDelay;
    friend class uiPopupWidget;
    uiPopupWidget* q_ptr;
};
}
#endif // UIPOPUPWIDGET_P_H