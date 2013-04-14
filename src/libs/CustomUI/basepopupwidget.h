#ifndef BASEPOPUPWIDGET_H
#define BASEPOPUPWIDGET_H

#include <QFrame>
#include <QEasingCurve>
#include <QtCore/QObject>
#include "customui_global.h"
/*!
 *  \class BasePopupWidget
 *  \brief Popup类窗口，提供三种动画效果
 */
namespace CustomUI{
class BasePopupWidgetPrivate;
class CUSTOMUI_EXPORT BasePopupWidget : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(QRect effectGeometry READ effectGeometry WRITE setEffectGeometry DESIGNABLE false)
    Q_PROPERTY(AnimationEffect animationEffect READ animationEffect WRITE setAnimationEffect)
    Q_PROPERTY(int effectDuration READ effectDuration WRITE setEffectDuration)
    Q_PROPERTY(QEasingCurve::Type easingCurve READ easingCurve WRITE setEasingCurve)
    Q_PROPERTY(double effectAlpha READ effectAlpha WRITE setEffectAlpha)
public:
    explicit BasePopupWidget(QWidget *parent = 0);
    virtual ~BasePopupWidget();
    ///
    /// Popup的动画效果
    enum AnimationEffect{
        WindowOpacityFadeEffect = 0,    //!< 强制窗口半透明
        ScrollEffect,                   //!< 滚动效果
        FadeEffect                      //!< 渐隐效果
    };
    enum VerticalDirection{
        TopToBottom = 1,
        BottomToTop = 2
    };

    QWidget* baseWidget() const;
    virtual void setBaseWidget(QWidget* baseWidget);

    QEasingCurve::Type easingCurve() const;
    void setEasingCurve(QEasingCurve::Type easingCurve);

    AnimationEffect animationEffect() const;
    void setAnimationEffect(AnimationEffect effect);

    QRect effectGeometry() const;   //!< 返回几何坐标
    double effectAlpha() const;     //!< 透明度



    Qt::Alignment alignment()const; //!< 设置弹出位置
    void setAlignment(Qt::Alignment alignment);//!< 设置弹出位置

    Qt::Orientations orientation()const;//!< 动画方向（从左到右等\sa Qt::Orientations）
    void setOrientation(Qt::Orientations orientation);//!< 设置动画方向（从左到右等\sa Qt::Orientations）

    int effectDuration()const;      //!< 动画播放时间
    void setEffectDuration(int duration);//!< 设置动画播放时间

    VerticalDirection verticalDirection()const;
    void setVerticalDirection(VerticalDirection direction);

    Qt::LayoutDirection horizontalDirection()const;
    void setHorizontalDirection(Qt::LayoutDirection direction);

protected slots:
    virtual void onEffectFinished();
    void setEffectGeometry(QRect geometry);
    void setEffectAlpha(double alpha);
    void onBaseWidgetDestroyed();
public slots:

    virtual void hidePopup();
    virtual void showPopup();
    virtual void showPopup(bool show);
signals:
    void popupOpened(bool open);

protected:
    explicit BasePopupWidget(BasePopupWidgetPrivate *pmpl, QWidget* parent);
    virtual bool event(QEvent* event);
    virtual void paintEvent(QPaintEvent*event);
    BasePopupWidgetPrivate *d_ptr;
private:
    friend class BasePopupWidgetPrivate;

};
}

#endif // BASEPOPUPWIDGET_H
