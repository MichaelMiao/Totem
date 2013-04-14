#ifndef BASEPOPUPWIDGET_P_H
#define BASEPOPUPWIDGET_P_H
#include "basepopupwidget.h"
#include "customui_global.h"
QT_BEGIN_NAMESPACE
class QPropertyAnimation;
class QWidget;
class QLabel;
QT_END_NAMESPACE

namespace CustomUI{

class CUSTOMUI_EXPORT BasePopupWidgetPrivate : public QObject
{

public:
    BasePopupWidgetPrivate(BasePopupWidget* object);
    virtual ~BasePopupWidgetPrivate();
    virtual void init();
    bool isOpening()const;
    bool isClosing() const;
    bool wasClosing() const;
    bool isAncestorOf(const QWidget* ancestor, const QWidget* child) const;
    void hideAll();
    QPropertyAnimation* currentAnimation()const;
    QRect desiredOpenGeometry()const;
    QRect desiredOpenGeometry(QRect baseGeometry)const;

    virtual QWidget* mouseOver();
    QWidgetList focusWidgets(bool onlyVisible = false)const;

    /// Return the closed geometry for the popup based on the current geometry
    QRect closedGeometry()const;
    /// Return the closed geometry for a given open geometry
    QRect   closedGeometry(QRect openGeom)const;
    void    setupPopupPixmapWidget();
    Qt::Alignment pixmapAlignment()const;

    QRect   baseGeometry()const;
    QPoint  mapToGlobal(const QPoint& baseWidgetPoint)const;
protected:
    BasePopupWidget::AnimationEffect    m_effect;

    bool                m_forcedTranslucent;
    int                 m_effectDuration;
    QPropertyAnimation* m_scrollAnimation;

    double              m_alpha;//透明度
    QPropertyAnimation* m_alphaAnimation;//透明度动画
    QWidget*            m_baseWidget;
    Qt::Alignment       m_alignment;
    Qt::Orientations    m_orientations;
    BasePopupWidget::VerticalDirection m_verticalDirection;
    Qt::LayoutDirection m_horizontalDirection;
    QLabel*             m_popupPixmapWidget;
    double              m_effectAlpha;
private:
    friend class BasePopupWidget;
    BasePopupWidget * q;
};
}
#endif // BASEPOPUPWIDGET_P_H