#ifndef UIPOPUPWIDGET_H
#define UIPOPUPWIDGET_H
#include "basepopupwidget.h"
#include "customui_global.h"

namespace CustomUI{
class uiPopupWidgetPrivate;
class CUSTOMUI_EXPORT uiPopupWidget : public BasePopupWidget
{
    Q_OBJECT
    Q_PROPERTY( bool active READ isActive WRITE setActive)
    Q_PROPERTY( bool autoShow READ autoShow WRITE setAutoShow)
    Q_PROPERTY( int showDelay READ showDelay WRITE setShowDelay)
    Q_PROPERTY( bool autoHide READ autoHide WRITE setAutoHide)
    Q_PROPERTY( int hideDelay READ hideDelay WRITE setHideDelay)
public:
    explicit uiPopupWidget(QWidget *parent = 0);
    virtual ~uiPopupWidget();
    bool isActive() const;          //!< 是否处于激活状态
    void setActive(bool bActive);   //!< 修改激活状态

    bool autoShow() const;
    void setAutoShow(bool bAutoShow);

    int showDelay() const;
    void setShowDelay(int iDelay);

    bool autoHide() const;
    void setAutoHide(bool bAutoHide);

    int hideDelay() const;
    void setHideDelay(int iDelay);  //!< 默认200ms
signals:

public slots:
    void pinPopup(bool pin);
public:
    virtual void hidePopup();
protected:
    virtual void leaveEvent(QEvent *event);
    virtual void enterEvent(QEvent *event);
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void setBaseWidget(QWidget *baseWidget);
protected slots:
    void updatePopup();
    virtual void onEffectFinished();
private:
    friend class uiPopupWidgetPrivate;
    uiPopupWidgetPrivate *d;
};
}

#endif // UIPOPUPWIDGET_H