#include "uipopupwidget.h"
#include "uipopupwidget_p.h"
#include <QEvent>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QLabel>
#include <QApplication>
#include <QAbstractAnimation>

namespace CustomUI{

uiPopupWidget::uiPopupWidget(QWidget *parent) :
    BasePopupWidget(new uiPopupWidgetPrivate(this), parent),
    d((uiPopupWidgetPrivate*)d_ptr)
{
    d->init();
}

uiPopupWidget::~uiPopupWidget()
{
}

bool uiPopupWidget::isActive() const
{
    return d->m_bActive;
}

void uiPopupWidget::setActive(bool bActive)
{
    if(d->m_bActive == bActive)
    {
        return ;
    }
    d->m_bActive = bActive;
    if(d->m_bActive)
    {
        if(d->m_baseWidget)
            d->m_baseWidget->installEventFilter(this);
        if(d->m_popupPixmapWidget)
            d->m_popupPixmapWidget->installEventFilter(this);
        qApp->installEventFilter(this);
    }
    else
    {
        if(d->m_baseWidget)
            d->m_baseWidget->removeEventFilter(this);
        if(d->m_popupPixmapWidget)
            d->m_popupPixmapWidget->removeEventFilter(this);
        qApp->removeEventFilter(this);
    }
}

bool uiPopupWidget::autoShow() const
{
    return d->m_bAutoShow;
}

void uiPopupWidget::setAutoShow(bool bAutoShow)
{
    d->m_bAutoShow = bAutoShow;
    QTimer::singleShot(d->m_iShowDelay,
                       this, SLOT(updatePopup()));
}

int uiPopupWidget::showDelay() const
{
    return d->m_iShowDelay;
}

void uiPopupWidget::setShowDelay(int iDelay)
{
    d->m_iShowDelay = iDelay;
}

bool uiPopupWidget::autoHide() const
{
    return d->m_bAutoHide;
}

void uiPopupWidget::setAutoHide(bool bAutoHide)
{
    d->m_bAutoHide = bAutoHide;
    QTimer::singleShot(d->m_iHideDelay, this,
                       SLOT(updatePopup()));
}

int uiPopupWidget::hideDelay() const
{
    return d->m_iHideDelay;
}

void uiPopupWidget::setHideDelay(int iDelay)
{
    d->m_iHideDelay = iDelay;
}

void uiPopupWidget::pinPopup(bool pin)
{
    this->setAutoHide(!pin);
    if (pin)
    {
        this->showPopup();
    }
    else
    {
        // When closing, we don't want to inadvertently re-open the menu.
        this->setProperty("AutoShowOnClose", this->autoShow());
        d->m_bAutoShow = false;
        this->hidePopup();
    }
}

void uiPopupWidget::hidePopup()
{
    this->setProperty("forcedClosed", 0);

    BasePopupWidget::hidePopup();
}

void uiPopupWidget::leaveEvent(QEvent *event)
{
    QTimer::singleShot(d->m_iHideDelay, this,
                       SLOT(updatePopup()));
    BasePopupWidget::leaveEvent(event);
}

void uiPopupWidget::enterEvent(QEvent *event)
{
    QTimer::singleShot(d->m_iShowDelay, this,
                       SLOT(updatePopup()));
    BasePopupWidget::enterEvent(event);
}

bool uiPopupWidget::eventFilter(QObject *obj, QEvent *event)
{
    switch(event->type())
    {
    case QEvent::Move:
    {
        if (obj != d->m_baseWidget)
        {
            break;
        }
        QMoveEvent* moveEvent = dynamic_cast<QMoveEvent*>(event);
        QRect newBaseGeometry = d->baseGeometry();
        newBaseGeometry.moveTopLeft(d->mapToGlobal(moveEvent->pos()));
        QRect desiredGeometry = d->desiredOpenGeometry(newBaseGeometry);
        this->move(desiredGeometry.topLeft());
        this->update();
        break;
    }
    case QEvent::Hide:
    case QEvent::Close:
        if (obj != d->m_baseWidget)
        {
            if (obj != d->m_popupPixmapWidget &&
                qobject_cast<QWidget*>(obj)->windowType() == Qt::Popup)
            {
                obj->removeEventFilter(this);
                QTimer::singleShot(d->m_iHideDelay,
                                   this,
                                   SLOT(updatePopup()));
            }
            break;
        }
        d->temporarilyHiddenOn();
        break;
    case QEvent::Show:
        if (obj != d->m_baseWidget)
        {
            break;
        }
        this->setGeometry(d->desiredOpenGeometry());
        d->temporarilyHiddenOff();
        break;
    case QEvent::Resize:
        if (obj != d->m_baseWidget ||
                !(d->m_alignment & Qt::AlignJustify ||
                  (d->m_alignment & Qt::AlignTop && d->m_alignment & Qt::AlignBottom)) ||
                !(d->isOpening() || this->isVisible()))
        {
            break;
        }
        // TODO: bug when the effect is WindowOpacityFadeEffect
        this->setGeometry(d->desiredOpenGeometry());
        break;
    case QEvent::Enter:
        if ( d->currentAnimation()->state() == QAbstractAnimation::Stopped )
        {
            QTimer::singleShot(d->m_iShowDelay,
                               this, SLOT(updatePopup()));
        }
        else
        {
            this->updatePopup();
        }
        break;
    case QEvent::Leave:
        if (obj != d->m_baseWidget &&
                obj != d->m_popupPixmapWidget &&
                qobject_cast<QWidget*>(obj)->windowType() == Qt::Popup)
        {
            break;
        }
        QTimer::singleShot(d->m_iHideDelay,
                           this, SLOT(updatePopup()));
        if (obj != d->m_baseWidget &&
                obj != d->m_popupPixmapWidget)
        {
            obj->removeEventFilter(this);
        }
        break;
    default:
        break;
    }
    return this->QObject::eventFilter(obj, event);
}

void uiPopupWidget::setBaseWidget(QWidget *baseWidget)
{
    if (d->m_baseWidget)
    {
        d->m_baseWidget->removeEventFilter(this);
    }
    BasePopupWidget::setBaseWidget(baseWidget);
    if (d->m_baseWidget && d->m_bActive)
    {
        d->m_baseWidget->installEventFilter(this);
    }
    QTimer::singleShot(d->m_iShowDelay,
                       this, SLOT(updatePopup()));
}

void uiPopupWidget::updatePopup()
{
    QWidget* mouseOver = (d->m_bAutoShow || d->m_bAutoHide) ? d->mouseOver() : 0;
    if ((d->m_bAutoShow ||
         (d->m_bAutoHide && d->isClosing()
          && this->property("AutoShowOnClose").toBool()))
        &&
        mouseOver &&
        (!d->m_baseWidget || d->m_baseWidget->isEnabled()))
    {
        this->showPopup();
    }
    else if (d->m_bAutoHide && !mouseOver)
    {
        this->hidePopup();
    }
}

void uiPopupWidget::onEffectFinished()
{
    bool wasClosing = d->wasClosing();
    BasePopupWidget::onEffectFinished();
    if (wasClosing)
    {
        /// restore the AutoShow if needed.
        if (!this->property("AutoShowOnClose").isNull())
        {
            d->m_bAutoShow = this->property("AutoShowOnClose").toBool();
            this->setProperty("AutoShowOnClose", QVariant());
        }
    }
}

//////////////////////////////////////////////////////////
//uiPopupWidgetPrivate
//////////////////////////////////////////////////////////
uiPopupWidgetPrivate::uiPopupWidgetPrivate(
        uiPopupWidget *pObj)
    : BasePopupWidgetPrivate(pObj), q_ptr(pObj)
{
    m_bActive   = false;
    m_bAutoShow = true;
    m_iShowDelay= 20;
    m_bAutoHide = false;
    m_iHideDelay= 200;
}

uiPopupWidgetPrivate::~uiPopupWidgetPrivate()
{

}

void uiPopupWidgetPrivate::init()
{
    this->setParent(q_ptr);
    q_ptr->setActive(true);
    BasePopupWidgetPrivate::init();
}

QWidget *uiPopupWidgetPrivate::mouseOver()
{
    QWidget* widgetUnderCursor = BasePopupWidgetPrivate::mouseOver();
    if (widgetUnderCursor &&
            !this->focusWidgets(true).contains(widgetUnderCursor))
    {
        widgetUnderCursor->installEventFilter(q_ptr);
    }
    return widgetUnderCursor;
}

bool uiPopupWidgetPrivate::eventFilter(QObject *obj, QEvent *event)
{
    QWidget* widget = qobject_cast<QWidget*>(obj);
    if (event->type() == QEvent::ApplicationDeactivate)
    {
        QTimer::singleShot(0, this, SLOT(onApplicationDeactivate()));
    }
    else if (event->type() == QEvent::ApplicationActivate)
    {
        QTimer::singleShot(0, this, SLOT(updateVisibility()));
    }
    if (!m_baseWidget)
    {
        return false;
    }
    if (event->type() == QEvent::Move && widget != m_baseWidget)
    {
        if (widget->isAncestorOf(m_baseWidget))
        {
            QMoveEvent* moveEvent = dynamic_cast<QMoveEvent*>(event);
            QPoint topLeft = widget->parentWidget() ? widget->parentWidget()->mapToGlobal(moveEvent->pos())
                                                    : moveEvent->pos();
            topLeft += m_baseWidget->mapTo(widget, QPoint(0,0));
            QRect newBaseGeometry = baseGeometry();
            newBaseGeometry.moveTopLeft(topLeft);
            QRect desiredGeometry = desiredOpenGeometry(newBaseGeometry);
            q_ptr->move(desiredGeometry.topLeft());
        }
        else if (widget->isWindow() &&
                 widget->windowType() != Qt::ToolTip &&
                 widget->windowType() != Qt::Popup)
        {
            QTimer::singleShot(0, this, SLOT(updateVisibility()));
        }
    }
    else if (event->type() == QEvent::Resize)
    {
        if (widget->isWindow() &&
                widget != m_baseWidget->window() &&
                widget->windowType() != Qt::ToolTip &&
                widget->windowType() != Qt::Popup)
        {
            QTimer::singleShot(0, this, SLOT(updateVisibility()));
        }
    }
    else if (event->type() == QEvent::WindowStateChange &&
             widget != m_baseWidget->window() &&
             widget->windowType() != Qt::ToolTip &&
             widget->windowType() != Qt::Popup)
    {
        QTimer::singleShot(0, this, SLOT(updateVisibility()));
    }
    else if ((event->type() == QEvent::WindowActivate ||
              event->type() == QEvent::WindowDeactivate) &&
             widget == this->m_baseWidget->window())
    {
        QTimer::singleShot(0, this, SLOT(updateVisibility()));
    }
    return false;
}

void uiPopupWidgetPrivate::temporarilyHiddenOn()
{
    if (!this->m_bAutoHide &&
            (q_ptr->isVisible() || this->isOpening()) &&
            !(q_ptr->isHidden() || this->isClosing()))
    {
        this->setProperty("forcedClosed",
                          this->isOpening() ? 2 : 1);
    }
    this->currentAnimation()->stop();
    this->hideAll();
}

void uiPopupWidgetPrivate::temporarilyHiddenOff()
{
    int forcedClosed = this->property("forcedClosed").toInt();
    if (forcedClosed > 0)
    {
        q_ptr->show();
        if (forcedClosed == 2)
        {
            emit q_ptr->popupOpened(true);
        }
        this->setProperty("forcedClosed", 0);
    }
    else
    {
        q_ptr->updatePopup();
    }
}

void uiPopupWidgetPrivate::updateVisibility()
{
    if (!this->m_baseWidget
        ||
        (!this->m_baseWidget->window()->isActiveWindow()
          &&
          (!qApp->activeWindow()
            ||
            (qApp->activeWindow()->windowType() != Qt::ToolTip
              &&
              qApp->activeWindow()->windowType() != Qt::Popup))))
    {
        foreach(QWidget* topLevelWidget, qApp->topLevelWidgets())
        {
            if (topLevelWidget->isVisible() &&
                    !(topLevelWidget->windowState() & Qt::WindowMinimized) &&
                    topLevelWidget->windowType() != Qt::ToolTip &&
                    topLevelWidget->windowType() != Qt::Popup &&
                    topLevelWidget != (this->m_baseWidget ? this->m_baseWidget->window() : 0) &&
                    topLevelWidget->frameGeometry().intersects(q_ptr->geometry()))
            {
                qDebug() << "hide" << q_ptr << "because of: " << topLevelWidget
                         << " with windowType: " << topLevelWidget->windowType()
                         << topLevelWidget->isVisible()
                         << (this->m_baseWidget ? this->m_baseWidget->window() : 0)
                         << topLevelWidget->frameGeometry();
                this->temporarilyHiddenOn();
                return;
            }
        }
    }
    // If the base widget is hidden or minimized, we don't want to restore the
    // popup.
    if (m_baseWidget &&
            (!m_baseWidget->isVisible() ||
             m_baseWidget->window()->windowState() & Qt::WindowMinimized))
    {
        return;
    }
    // Restore the visibility of the popup if it was hidden
    this->temporarilyHiddenOff();
}

void uiPopupWidgetPrivate::onApplicationDeactive()
{
    if (!qApp->activeWindow())
    {
        this->temporarilyHiddenOn();
    }
}

}