#include "basepopupwidget.h"
#include "basepopupwidget_p.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QAbstractAnimation>
#include <qglobal.h>
#include <QEvent>
#include <QLabel>
#include <QApplication>
#include <QVBoxLayout>
#include <QWidgetList>

namespace CustomUI{
QGradient* duplicateGradient(const QGradient* gradient)
{
    QGradient* newGradient = 0;
    switch (gradient->type())
    {
    case QGradient::LinearGradient:
    {
        const QLinearGradient* linearGradient = static_cast<const QLinearGradient*>(gradient);
        newGradient = new QLinearGradient(linearGradient->start(), linearGradient->finalStop());
        break;
    }
    case QGradient::RadialGradient:
    {
        const QRadialGradient* radialGradient = static_cast<const QRadialGradient*>(gradient);
        newGradient = new QRadialGradient(radialGradient->center(), radialGradient->radius());
        break;
    }
    case QGradient::ConicalGradient:
    {
        const QConicalGradient* conicalGradient = static_cast<const QConicalGradient*>(gradient);
        newGradient = new QConicalGradient(conicalGradient->center(), conicalGradient->angle());
        break;
    }
    default:
        break;
    }
    if (!newGradient)
    {
        Q_ASSERT(gradient->type() != QGradient::NoGradient);
        return newGradient;
    }
    newGradient->setCoordinateMode(gradient->coordinateMode());
    newGradient->setSpread(gradient->spread());
    newGradient->setStops(gradient->stops());
    return newGradient;
}

BasePopupWidget::BasePopupWidget(QWidget* parent) :
    QFrame(parent, Qt::ToolTip | Qt::FramelessWindowHint),
    d_ptr(new BasePopupWidgetPrivate(this))
{
    d_ptr->init();
}

BasePopupWidget::BasePopupWidget(BasePopupWidgetPrivate *pmpl,
                                 QWidget *parent)
    : QFrame(parent,
             Qt::ToolTip | Qt::FramelessWindowHint),
      d_ptr(pmpl)
{

}

BasePopupWidget::~BasePopupWidget()
{
}

QWidget *BasePopupWidget::baseWidget() const
{
    return d_ptr->m_baseWidget;
}

void BasePopupWidget::setBaseWidget(QWidget *baseWidget)
{
    if (d_ptr->m_baseWidget)
    {
        //disconnect(d_ptr->BaseWidget, SIGNAL(destroyed(QObject*)),
        //           this, SLOT(onBaseWidgetDestroyed()));
    }
    d_ptr->m_baseWidget = baseWidget;
    if (d_ptr->m_baseWidget)
    {
        //connect(d_ptr->BaseWidget, SIGNAL(destroyed(QObject*)),
        //        this, SLOT(onBaseWidgetDestroyed()));
    }
}

QEasingCurve::Type BasePopupWidget::easingCurve() const
{
    return d_ptr->m_scrollAnimation->easingCurve().type();
}

void BasePopupWidget::setEasingCurve(QEasingCurve::Type easingCurve)
{
    d_ptr->m_scrollAnimation->setEasingCurve(easingCurve);
    d_ptr->m_alphaAnimation->setEasingCurve(easingCurve);
}

void BasePopupWidget::setAnimationEffect(CustomUI::BasePopupWidget::AnimationEffect effect)
{
    d_ptr->m_effect = effect;
}

QRect BasePopupWidget::effectGeometry() const
{
    return d_ptr->m_popupPixmapWidget->geometry();
}

void BasePopupWidget::setEffectGeometry(QRect geometry)
{
    d_ptr->m_popupPixmapWidget->setGeometry(geometry);
    d_ptr->m_popupPixmapWidget->repaint();
}

double BasePopupWidget::effectAlpha() const
{
    return d_ptr->m_alpha;
}

Qt::Alignment BasePopupWidget::alignment() const
{
    return d_ptr->m_alignment;
}

void BasePopupWidget::setAlignment(Qt::Alignment alignment)
{
    d_ptr->m_alignment = alignment;
}

Qt::Orientations BasePopupWidget::orientation() const
{
    return d_ptr->m_orientations;
}

void BasePopupWidget::setOrientation(Qt::Orientations orientation)
{
    d_ptr->m_orientations = orientation;
}

int BasePopupWidget::effectDuration() const
{
    return d_ptr->m_effectDuration;
}

void BasePopupWidget::setEffectDuration(int duration)
{
    d_ptr->m_effectDuration = duration;
    d_ptr->m_scrollAnimation->setDuration(duration);
    d_ptr->m_alphaAnimation->setDuration(duration);
}

BasePopupWidget::VerticalDirection BasePopupWidget::verticalDirection() const
{
    return d_ptr->m_verticalDirection;
}

void BasePopupWidget::setVerticalDirection(BasePopupWidget::VerticalDirection direction)
{
    d_ptr->m_verticalDirection = direction;
}

Qt::LayoutDirection BasePopupWidget::horizontalDirection() const
{
    return d_ptr->m_horizontalDirection;
}

void BasePopupWidget::setHorizontalDirection(Qt::LayoutDirection direction)
{
    d_ptr->m_horizontalDirection = direction;
}


void BasePopupWidget::onEffectFinished()
{
    if(d_ptr->m_forcedTranslucent)
    {
        d_ptr->m_forcedTranslucent = false;
        this->setAttribute(Qt::WA_TranslucentBackground, true);
    }
    if(d_ptr->wasClosing())
    {
        d_ptr->hideAll();
        emit popupOpened(false);
    }
    else
    {
        this->show();
        emit popupOpened(true);
    }
}

void BasePopupWidget::setEffectAlpha(double alpha)
{
    d_ptr->m_alpha = alpha;
}

void BasePopupWidget::onBaseWidgetDestroyed()
{
    d_ptr->hideAll();
    this->setBaseWidget(0);
    this->deleteLater();
}

void BasePopupWidget::showPopup(bool show)
{
    if(show)
    {
        this->showPopup();
    }
    else
    {
        this->hidePopup();
    }
}

bool BasePopupWidget::event(QEvent *event)
{
    switch(event->type())
    {
    case QEvent::ParentChange:
        break;
    default:
        break;
    }
    return QFrame::event(event);
}

void BasePopupWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QBrush brush = this->palette().window();
    if (brush.style() == Qt::LinearGradientPattern ||
            brush.style() == Qt::ConicalGradientPattern ||
            brush.style() == Qt::RadialGradientPattern)
    {
        QGradient* newGradient = duplicateGradient(brush.gradient());
        QGradientStops stops;
        foreach(QGradientStop stop, newGradient->stops())
        {
            stop.second.setAlpha(stop.second.alpha() * d_ptr->m_effectAlpha);
            stops.push_back(stop);
        }
        newGradient->setStops(stops);
        brush = QBrush(*newGradient);
        delete newGradient;
    }
    else
    {
        QColor color = brush.color();
        color.setAlpha(color.alpha() * d_ptr->m_effectAlpha);
        brush.setColor(color);
    }
    //QColor semiTransparentColor = this->palette().window().color();
    //semiTransparentColor.setAlpha(d_ptr->CurrentAlpha);
    painter.fillRect(this->rect(), brush);
    painter.end();
    // Let the QFrame draw itself if needed
    QFrame::paintEvent(event);
}

void BasePopupWidget::showPopup()
{
    if ((this->isVisible() &&
         d_ptr->currentAnimation()->state() == QAbstractAnimation::Stopped)
            ||(d_ptr->m_baseWidget && !d_ptr->m_baseWidget->isVisible()))
    {
        return;
    }

    /*！
     *  重新布局，以保证可以获得Geometry
     */
    if (this->layout() && !this->testAttribute(Qt::WA_WState_Created))
    {
        this->layout()->activate();
    }
    this->setGeometry(d_ptr->desiredOpenGeometry());
    /// Maybe the popup doesn't allow the desiredOpenGeometry if the widget
    /// minimum size is larger than the desired size.
    QRect openGeometry = this->geometry();
    QRect closedGeometry = d_ptr->closedGeometry();

    d_ptr->currentAnimation()->setDirection(QAbstractAnimation::Forward);

    switch(d_ptr->m_effect)
    {
    case WindowOpacityFadeEffect:
        if (!this->testAttribute(Qt::WA_TranslucentBackground))
        {
            d_ptr->m_forcedTranslucent = true;
            this->setAttribute(Qt::WA_TranslucentBackground, true);
        }
        this->show();
        break;
    case ScrollEffect:
    {
        d_ptr->m_popupPixmapWidget->setGeometry(closedGeometry);
        d_ptr->m_scrollAnimation->setStartValue(closedGeometry);
        d_ptr->m_scrollAnimation->setEndValue(openGeometry);
        d_ptr->setupPopupPixmapWidget();
        d_ptr->m_popupPixmapWidget->show();
        break;
    }
    default:
        break;
    }
    switch(d_ptr->currentAnimation()->state())
    {
    case QAbstractAnimation::Stopped:
        d_ptr->currentAnimation()->start();
        break;
    case QAbstractAnimation::Paused:
        d_ptr->currentAnimation()->resume();
        break;
    default:
    case QAbstractAnimation::Running:
        break;
    }
}

void BasePopupWidget::hidePopup()
{
    if (!this->isVisible()
        && d_ptr->currentAnimation()->state() == QAbstractAnimation::Stopped)
    {
        return;
    }
    d_ptr->currentAnimation()->setDirection(QAbstractAnimation::Backward);

    QRect openGeometry = this->geometry();
    QRect closedGeometry = d_ptr->closedGeometry();

    switch(d_ptr->m_effect)
    {
    case WindowOpacityFadeEffect:
        if (!this->testAttribute(Qt::WA_TranslucentBackground))
        {
            d_ptr->m_forcedTranslucent = true;
            this->setAttribute(Qt::WA_TranslucentBackground, true);
        }
        break;
    case ScrollEffect:
    {
        d_ptr->m_scrollAnimation->setStartValue(closedGeometry);
        d_ptr->m_scrollAnimation->setEndValue(openGeometry);
        d_ptr->setupPopupPixmapWidget();
        d_ptr->m_popupPixmapWidget->setGeometry(this->geometry());
        d_ptr->m_popupPixmapWidget->show();
        if (this->isActiveWindow())
        {
            qApp->setActiveWindow(d_ptr->m_baseWidget ? d_ptr->m_baseWidget->window() : 0);
        }
        this->hide();
        break;
    }
    default:
        break;
    }
    switch(d_ptr->currentAnimation()->state())
    {
    case QAbstractAnimation::Stopped:
        d_ptr->currentAnimation()->start();
        break;
    case QAbstractAnimation::Paused:
        d_ptr->currentAnimation()->resume();
        break;
    default:
    case QAbstractAnimation::Running:
        break;
    }
}

BasePopupWidget::AnimationEffect BasePopupWidget::animationEffect() const
{
    return d_ptr->m_effect;
}

BasePopupWidgetPrivate::BasePopupWidgetPrivate(BasePopupWidget *object)
    : q(object)
{
    this->m_effect = BasePopupWidget::ScrollEffect;
    this->m_effectDuration    = 300;
    m_baseWidget            = 0;
    m_effectAlpha           = 1;
    m_forcedTranslucent     = false;
    m_scrollAnimation       = 0;
    m_popupPixmapWidget     = 0;
    m_alignment             = Qt::AlignJustify | Qt::AlignBottom;
    m_orientations          = Qt::Vertical;
    m_verticalDirection     = BasePopupWidget::TopToBottom;
    m_horizontalDirection   = Qt::LeftToRight;

}

BasePopupWidgetPrivate::~BasePopupWidgetPrivate()
{
}

void BasePopupWidgetPrivate::init()
{
    //显示ToolTip
    q->setAttribute(Qt::WA_AlwaysShowToolTips, true);

    m_alphaAnimation    = new QPropertyAnimation(q, "effectAlpha", q);
    m_alphaAnimation->setDuration(m_effectDuration);
    m_alphaAnimation->setStartValue(0.0);
    m_alphaAnimation->setEndValue(1.0);
    connect(m_alphaAnimation, SIGNAL(finished()),
            q, SLOT(onEffectFinished()));

    m_popupPixmapWidget = new QLabel(q, Qt::ToolTip | Qt::FramelessWindowHint);
    m_scrollAnimation = new QPropertyAnimation(q, "effectGeometry", q);
    m_scrollAnimation->setDuration(m_effectDuration);
    connect(m_scrollAnimation, SIGNAL(finished()),
            q, SLOT(onEffectFinished()));
    connect(m_scrollAnimation, SIGNAL(finished()),
            m_popupPixmapWidget, SLOT(hide()));

    q->setAnimationEffect(this->m_effect);
    q->setEasingCurve(QEasingCurve::OutCurve);
    q->setBaseWidget(q->parentWidget());
}

bool BasePopupWidgetPrivate::isOpening() const
{
    return this->currentAnimation()->state() == QAbstractAnimation::Running &&
      this->currentAnimation()->direction() == QAbstractAnimation::Forward;
}

bool BasePopupWidgetPrivate::isClosing() const
{
    return currentAnimation()->state() == QAbstractAnimation::Running &&
            currentAnimation()->direction() == QAbstractAnimation::Backward;
}

bool BasePopupWidgetPrivate::wasClosing() const
{
    QAbstractAnimation *pAnimation = qobject_cast<QAbstractAnimation*>(q->sender());
    return pAnimation->direction() == QAbstractAnimation::Backward;
}

bool BasePopupWidgetPrivate::isAncestorOf(const QWidget* ancestor, const QWidget* child) const
{
    while(child)
    {
        if(child == ancestor)
        {
            return true;
        }
        child = child->parentWidget();
    }
    return false;
}

void BasePopupWidgetPrivate::hideAll()
{
    if(!(q->windowFlags() & Qt::ToolTip))
    {
        return;
    }
    if(q->isActiveWindow() && m_baseWidget)
    {
        qApp->setActiveWindow(m_baseWidget->window());
    }
    q->hide();
    m_popupPixmapWidget->hide();
    QWidget *activePopupWidget = qApp->activePopupWidget();
    if(activePopupWidget && this->isAncestorOf(q, activePopupWidget))
    {
        activePopupWidget->close();
    }
}

QPropertyAnimation *BasePopupWidgetPrivate::currentAnimation() const
{
    return m_effect == BasePopupWidget::ScrollEffect ?
                m_scrollAnimation : m_alphaAnimation;
}

QRect BasePopupWidgetPrivate::desiredOpenGeometry() const
{
    return this->desiredOpenGeometry(this->baseGeometry());
}

QRect BasePopupWidgetPrivate::desiredOpenGeometry(QRect baseGeometry) const
{
    QSize size = q->size();
    if (!q->testAttribute(Qt::WA_WState_Created))
    {
        size = q->sizeHint();
    }

    if (baseGeometry.isNull())
    {
        return QRect(q->pos(), size);
    }

    QRect geometry;
    if (m_alignment & Qt::AlignJustify)
    {
        if (m_orientations & Qt::Vertical)
        {
            size.setWidth(baseGeometry.width());
        }
    }
    if (m_alignment & Qt::AlignTop &&
            m_alignment & Qt::AlignBottom)
    {
        size.setHeight(baseGeometry.height());
    }

    geometry.setSize(size);

    QPoint topLeft = baseGeometry.topLeft();
    QPoint bottomRight = baseGeometry.bottomRight();

    if (m_alignment & Qt::AlignLeft)
    {
        if (m_horizontalDirection == Qt::LeftToRight)
        {
            geometry.moveLeft(topLeft.x());
        }
        else
        {
            geometry.moveRight(topLeft.x() - 1);
        }
    }
    else if (m_alignment & Qt::AlignRight)
    {
        if (m_horizontalDirection == Qt::LeftToRight)
        {
            geometry.moveLeft(bottomRight.x() + 1);
        }
        else
        {
            geometry.moveRight(bottomRight.x());
        }
    }
    else if (m_alignment & Qt::AlignHCenter)
    {
        geometry.moveLeft((topLeft.x() + bottomRight.x()) / 2 - size.width() / 2);
    }
    else if (m_alignment & Qt::AlignJustify)
    {
        geometry.moveLeft(topLeft.x());
    }

    if (m_alignment & Qt::AlignTop)
    {
        if (m_verticalDirection == BasePopupWidget::TopToBottom)
        {
            geometry.moveTop(topLeft.y());
        }
        else
        {
            geometry.moveBottom(topLeft.y() - 1);
        }
    }
    else if (m_alignment & Qt::AlignBottom)
    {
        if (m_verticalDirection == BasePopupWidget::TopToBottom)
        {
            geometry.moveTop(bottomRight.y() + 1);
        }
        else
        {
            geometry.moveBottom(bottomRight.y());
        }
    }
    else if (m_alignment & Qt::AlignVCenter)
    {
        geometry.moveTop((topLeft.y() + bottomRight.y()) / 2 - size.height() / 2);
    }
    return geometry;
}

QWidget *BasePopupWidgetPrivate::mouseOver()
{
    QList<QWidget*> widgets = focusWidgets(true);
    foreach(QWidget* widget, widgets)
    {
        if (widget->underMouse())
        {
            return widget;
        }
    }
    const QPoint pos = QCursor::pos();
    QWidget* widgetUnderCursor = qApp->widgetAt(pos);
    foreach(const QWidget* focusWidget, widgets)
    {
        if (this->isAncestorOf(focusWidget, widgetUnderCursor) &&
                (focusWidget != widgetUnderCursor ||
                 QRect(QPoint(0,0), focusWidget->size()).contains(
                     focusWidget->mapFromGlobal(pos))))
        {
            return widgetUnderCursor;
        }
    }
    return 0;
}

QWidgetList BasePopupWidgetPrivate::focusWidgets(bool onlyVisible) const
{
    QWidgetList res;
    if (!onlyVisible || q->isVisible())
    {
        res << const_cast<BasePopupWidget*>(q);
    }
    if (m_baseWidget && (!onlyVisible || m_baseWidget->isVisible()))
    {
        res << m_baseWidget;
    }
    if (m_popupPixmapWidget && (!onlyVisible || m_popupPixmapWidget->isVisible()))
    {
        res << m_popupPixmapWidget;
    }
    return res;
}

QRect BasePopupWidgetPrivate::closedGeometry() const
{
    return this->closedGeometry(q->geometry());
}

QRect BasePopupWidgetPrivate::closedGeometry(QRect openGeom) const
{
    if (this->m_orientations & Qt::Vertical)
    {
        if (this->m_verticalDirection == BasePopupWidget::BottomToTop)
        {
            openGeom.moveTop(openGeom.bottom());
        }
        openGeom.setHeight(0);
    }
    if (this->m_orientations & Qt::Horizontal)
    {
        if (this->m_horizontalDirection == Qt::RightToLeft)
        {
            openGeom.moveLeft(openGeom.right());
        }
        openGeom.setWidth(0);
    }
    return openGeom;
}

void BasePopupWidgetPrivate::setupPopupPixmapWidget()
{
    this->m_popupPixmapWidget->setAlignment(this->pixmapAlignment());
    QPixmap pixmap;
    if (q->testAttribute(Qt::WA_TranslucentBackground))
    {
        // only QImage handle transparency correctly
        QImage image(q->geometry().size(), QImage::Format_ARGB32);
        image.fill(0);
        q->render(&image);
        pixmap = QPixmap::fromImage(image);
    }
    else
    {
        pixmap = QPixmap::grabWidget(q, QRect(QPoint(0,0), q->geometry().size()));
    }
    this->m_popupPixmapWidget->setPixmap(pixmap);
    this->m_popupPixmapWidget->setAttribute(
                Qt::WA_TranslucentBackground,
                q->testAttribute(Qt::WA_TranslucentBackground));
    this->m_popupPixmapWidget->setWindowOpacity(q->windowOpacity());
}

Qt::Alignment BasePopupWidgetPrivate::pixmapAlignment() const
{
    Qt::Alignment alignment;
    if (this->m_verticalDirection == BasePopupWidget::TopToBottom)
    {
        alignment |= Qt::AlignBottom;
    }
    else
    {
        alignment |= Qt::AlignTop;
    }

    if (this->m_horizontalDirection == Qt::LeftToRight)
    {
        alignment |= Qt::AlignRight;
    }
    else// if (this->VerticalDirection == ctkBasePopupWidget::BottomToTop)
    {
        alignment |= Qt::AlignLeft;
    }
    return alignment;
}

QRect BasePopupWidgetPrivate::baseGeometry() const
{
    if (!this->m_baseWidget)
    {
        return QRect();
    }
    return QRect(this->mapToGlobal(this->m_baseWidget->geometry().topLeft()),
                 this->m_baseWidget->size());
}

QPoint BasePopupWidgetPrivate::mapToGlobal(const QPoint &baseWidgetPoint) const
{
    QPoint mappedPoint = baseWidgetPoint;
    if (this->m_baseWidget && this->m_baseWidget->parentWidget())
    {
        mappedPoint = this->m_baseWidget->parentWidget()->mapToGlobal(mappedPoint);
    }
    return mappedPoint;
}

}//namespace CustomUI
