#include "collapsiblebutton.h"
#include <QStyleOptionButton>
#include <QPaintEvent>
#include <QPainter>
#include <QApplication>
namespace CustomUI{

class CollapsibleButtonPrivate
{
protected:
    friend class CollapsibleButton;
    CollapsibleButton* const q_ptr;
public:
    CollapsibleButtonPrivate(CollapsibleButton& object);
    void init();
    void setChildVisibility(QWidget* childWidget);

    bool     m_bCollapsed;

    int      m_collapsedHeight;
    bool     m_bExclusiveMouseOver;
    bool     m_bLookOffWhenChecked;
    bool     m_bForcingVisibility;
    bool     m_bStateCreated;
    QFrame::Shape   m_contentsFrameShape;
    QFrame::Shadow  m_contentsFrameShadow;

    int      m_maximumHeight;

    Qt::Alignment m_textAlignment;
    Qt::Alignment m_indicatorAlignment;
};


CollapsibleButton::CollapsibleButton(QWidget* parent) :
    QAbstractButton(parent),
    d(new CollapsibleButtonPrivate(*this))
{
    d->init();
}

CollapsibleButton::CollapsibleButton(const QString &text, QWidget *parent)
    : QAbstractButton(parent),
      d(new CollapsibleButtonPrivate(*this))
{
    d->init();
    setText(text);
}

CollapsibleButton::~CollapsibleButton()
{
}

bool CollapsibleButton::isCollapsed()
{
    return d->m_bCollapsed;
}

void CollapsibleButton::setCollapsed(bool bCollapsed)
{
    if (!this->isCheckable())
    {
        this->collapse(bCollapsed);
        return;
    }
    this->setChecked(!bCollapsed);
}

Qt::Alignment CollapsibleButton::buttonTextAlignment()
{
    return d->m_textAlignment;
}

void CollapsibleButton::setButtonTextAlignment(Qt::Alignment alignment)
{
    d->m_textAlignment = alignment;
}

Qt::Alignment CollapsibleButton::indicatorAlignment() const
{
    return d->m_indicatorAlignment;
}

void CollapsibleButton::setIndicatorAlignment(Qt::Alignment indicatorAlignment)
{
    d->m_indicatorAlignment = indicatorAlignment;
}

QSize CollapsibleButton::minimumSizeHint() const
{
    QSize buttonSize = this->buttonSizeHint();
    if (d->m_bCollapsed)
    {
        return buttonSize + QSize(0,d->m_collapsedHeight);
    }
    // open
    if (this->layout() == 0)
    {
        return buttonSize;
    }
    QSize s = this->QAbstractButton::minimumSizeHint();
    return s.expandedTo(buttonSize);
}

QSize CollapsibleButton::sizeHint() const
{
    QSize buttonSize = this->buttonSizeHint();
    if (d->m_bCollapsed)
    {
        return buttonSize + QSize(0,d->m_collapsedHeight);
    }
    QSize s = this->QAbstractButton::sizeHint();
    return s.expandedTo(buttonSize + QSize(0, d->m_collapsedHeight));
}

bool CollapsibleButton::event(QEvent *event)
{
    if(event->type() == QEvent::StyleChange
            || event->type() == QEvent::FontChange)
    {
        this->setContentsMargins(0, this->buttonSizeHint().height(), 0, 0);
    }
    if(isCollapsed())
    {
        this->setMaximumHeight(this->sizeHint().height());
    }
    return QAbstractButton::event(event);
}

bool CollapsibleButton::eventFilter(QObject *child, QEvent *e)
{
    if(d->m_bForcingVisibility)
    {
        return false;
    }
    if(e->type() == QEvent::ShowToParent)
    {
        child->setProperty("visibilityToParent", true);
        d->setChildVisibility(qobject_cast<QWidget*>(child));
    }
    else if(e->type() == QEvent::HideToParent)
    {
        child->setProperty("visibilityToParent", false);
    }
    return QWidget::eventFilter(child, e);
}

void CollapsibleButton::setVisible(bool bShow)
{
    d->m_bForcingVisibility = true;
    QWidget::setVisible(bShow);
    d->m_bForcingVisibility = false;
    if(!d->m_bStateCreated && this->testAttribute(Qt::WA_WState_Created))
    {
        d->m_bStateCreated = true;
        foreach(QObject* child, this->children())
        {
            QWidget* childWidget = qobject_cast<QWidget*>(child);
            if(childWidget)
            {
                d->setChildVisibility(childWidget);
            }
        }
    }
}


void CollapsibleButton::collapse(bool c)
{
    if(c == d->m_bCollapsed)
    {
        return;
    }
    d->m_bCollapsed = c;
    if(c)
    {
        d->m_maximumHeight = this->maximumHeight();
        setMaximumHeight(this->sizeHint().height());

    }
    else
    {
        setMaximumHeight(d->m_maximumHeight);
        updateGeometry();
    }
    foreach(QObject* child, this->children())
    {
        QWidget* childWidget = qobject_cast<QWidget*>(child);
        if(childWidget)
        {
            d->setChildVisibility(childWidget);
        }
    }
    QWidget* parent = this->parentWidget();
    if(!d->m_bCollapsed && (!parent || !parent->layout()))
    {
        this->resize(this->sizeHint());
    }
    else
    {
        this->updateGeometry();
    }
    emit contentsCollapsed(c);
}

void CollapsibleButton::onToggled(bool clicked)
{
    if(this->isCheckable())
    {
        this->collapse(!clicked);
    }
}

void CollapsibleButton::paintEvent(QPaintEvent *_event)
{
    QPainter p(this);
    QStyleOptionButton opt;
    this->initStyleOption(&opt);

    bool exclusiveMouseOver = false;
    if (opt.state & QStyle::State_MouseOver)
    {
        QRect buttonRect = opt.rect;
        QList<QWidget*> _children = this->findChildren<QWidget*>();
        QList<QWidget*>::ConstIterator it;
        for (it = _children.constBegin(); it != _children.constEnd(); ++it )
        {
            if ((*it)->underMouse())
            {
                if (!_event->rect().contains(buttonRect))
                {
                    this->update(buttonRect);
                }
                opt.state &= ~QStyle::State_MouseOver;
                exclusiveMouseOver = true;
                break;
            }
        }
        if (d->m_bExclusiveMouseOver && !exclusiveMouseOver)
        {
            if (!_event->rect().contains(buttonRect))
            {
                this->update(buttonRect);
            }
        }
    }
    d->m_bExclusiveMouseOver = exclusiveMouseOver;
    QSize indicatorSize = QSize(style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, this),
                                style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, this));
    opt.iconSize = indicatorSize;
    style()->drawControl(QStyle::CE_PushButtonBevel, &opt, &p, this);
    int buttonHeight = opt.rect.height();
    uint tf = d->m_textAlignment;
    if (this->style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
    {
        tf |= Qt::TextShowMnemonic;
    }
    else
    {
        tf |= Qt::TextHideMnemonic;
    }
    int textWidth = opt.fontMetrics.boundingRect(opt.rect, tf, opt.text).width();
    int indicatorSpacing = this->style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, this);
    int buttonMargin = this->style()->pixelMetric(QStyle::PM_ButtonMargin, &opt, this);

    QStyleOption indicatorOpt;
    indicatorOpt.init(this);
    if (d->m_indicatorAlignment & Qt::AlignLeft)
    {
        indicatorOpt.rect = QRect((buttonHeight - indicatorSize.width()) / 2,
                                  (buttonHeight - indicatorSize.height()) / 2,
                                  indicatorSize.width(), indicatorSize.height());
    }
    else if (d->m_indicatorAlignment & Qt::AlignHCenter)
    {
        int w = indicatorSize.width();
        if (!opt.text.isEmpty() && (d->m_textAlignment & Qt::AlignHCenter))
        {
            w += textWidth + indicatorSpacing;
        }
        indicatorOpt.rect = QRect(opt.rect.x()+ opt.rect.width() /2 - w / 2,
                                  (buttonHeight - indicatorSize.height()) / 2,
                                  indicatorSize.width(), indicatorSize.height());
        if (d->m_textAlignment & Qt::AlignLeft &&
                indicatorOpt.rect.left() < opt.rect.x() + buttonMargin + textWidth)
        {
            indicatorOpt.rect.moveLeft(opt.rect.x() + buttonMargin + textWidth);
        }
        else if (d->m_textAlignment & Qt::AlignRight &&
                 indicatorOpt.rect.right() > opt.rect.right() - buttonMargin - textWidth)
        {
            indicatorOpt.rect.moveRight(opt.rect.right() - buttonMargin - textWidth);
        }
    }
    else if (d->m_indicatorAlignment & Qt::AlignRight)
    {
        indicatorOpt.rect = QRect(opt.rect.width() - (buttonHeight - indicatorSize.width()) / 2
                                  - indicatorSize.width(),
                                  (buttonHeight - indicatorSize.height()) / 2,
                                  indicatorSize.width(), indicatorSize.height());
    }
    if (d->m_bCollapsed)
    {
        style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &indicatorOpt, &p, this);
    }
    else
    {
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &indicatorOpt, &p, this);
    }

    // Draw Text
    if (d->m_textAlignment & Qt::AlignLeft)
    {
        if (d->m_indicatorAlignment & Qt::AlignLeft)
        {
            opt.rect.setLeft(indicatorOpt.rect.right() + indicatorSpacing);
        }
        else
        {
            opt.rect.setLeft(opt.rect.x() + buttonMargin);
        }
    }
    else if (d->m_textAlignment & Qt::AlignHCenter)
    {
        if (d->m_indicatorAlignment & Qt::AlignHCenter)
        {
            opt.rect.setLeft(indicatorOpt.rect.right() + indicatorSpacing);
        }
        else
        {
            opt.rect.setLeft(opt.rect.x() + opt.rect.width() / 2 - textWidth / 2);
            if (d->m_indicatorAlignment & Qt::AlignLeft)
            {
                opt.rect.setLeft( qMax(indicatorOpt.rect.right() + indicatorSpacing, opt.rect.left()) );
            }
        }
    }
    else if (d->m_textAlignment & Qt::AlignRight)
    {
        if (d->m_indicatorAlignment & Qt::AlignRight)
        {
            opt.rect.setLeft(indicatorOpt.rect.left() - indicatorSpacing - textWidth);
        }
        else
        {
            opt.rect.setLeft(opt.rect.right() - buttonMargin - textWidth);
        }
    }
    // all the computations have been made infering the text would be left oriented
    tf &= ~Qt::AlignHCenter & ~Qt::AlignRight;
    tf |= Qt::AlignLeft;
    style()->drawItemText(&p, opt.rect, tf, opt.palette, (opt.state & QStyle::State_Enabled),
                          opt.text, QPalette::ButtonText);

    // Draw Frame around contents
    QStyleOptionFrameV3 fopt;
    fopt.init(this);
    // HACK: on some styles, the frame doesn't exactly touch the button.
    // this is because the button has some kind of extra border.
	fopt.rect.setTop(buttonHeight);
    fopt.frameShape = d->m_contentsFrameShape;
    switch (d->m_contentsFrameShadow)
    {
    case QFrame::Sunken:
        fopt.state |= QStyle::State_Sunken;
        break;
    case QFrame::Raised:
        fopt.state |= QStyle::State_Raised;
        break;
    default:
    case QFrame::Plain:
        break;
    }
    style()->drawControl(QStyle::CE_ShapedFrame, &fopt, &p, this);
}

void CollapsibleButton::childEvent(QChildEvent *c)
{
    QObject* child = c->child();
    if(c && c->type() == QEvent::ChildAdded
            && child && child->isWidgetType())
    {
        QWidget* childWidget = qobject_cast<QWidget*>(c->child());
        if(childWidget->testAttribute(Qt::WA_WState_ExplicitShowHide)
                && childWidget->testAttribute(Qt::WA_WState_Hidden))
        {
            childWidget->setProperty("visibilityToParent", false);
        }
        child->installEventFilter(this);
        d->setChildVisibility(childWidget);
    }
    QAbstractButton::childEvent(c);
}

bool CollapsibleButton::hitButton(const QPoint &pos) const
{
    QStyleOptionButton opt;
    this->initStyleOption(&opt);
    return opt.rect.contains(pos);
}

QSize CollapsibleButton::buttonSizeHint() const
{
    int w = 0, h = 0;
    QStyleOptionButton opt;
    opt.initFrom(this);
    QSize indicatorSize = QSize(style()->pixelMetric(QStyle::PM_IndicatorWidth, &opt, this),
                                style()->pixelMetric(QStyle::PM_IndicatorHeight, &opt, this));
    int indicatorSpacing = style()->pixelMetric(QStyle::PM_CheckBoxLabelSpacing, &opt, this);
    int ih = indicatorSize.height();
    int iw = indicatorSize.width() + indicatorSpacing;
    w += iw;
    h = qMax(h, ih);

    QString string(this->text());
    bool empty = string.isEmpty();
    if(empty)
    {
        string = QString::fromLatin1("XXXX");
    }
    QFontMetrics fm = this->fontMetrics();
    QSize sz = fm.size(Qt::TextShowMnemonic, string);
    if(!empty || !w)
    {
        w += sz.width();
    }
    h = qMax(h, sz.height());
    QSize buttonSize = (style()->sizeFromContents(QStyle::CT_PushButton, &opt, QSize(w, h), this).
                          expandedTo(QApplication::globalStrut()));
    return buttonSize;
}

void CollapsibleButton::initStyleOption(QStyleOptionButton *option) const
{
    if (option == 0)
    {
        return;
    }
    option->initFrom(this);

    if (this->isDown() )
    {
        option->state |= QStyle::State_Sunken;
    }
    if (this->isChecked() && !d->m_bLookOffWhenChecked)
    {
        option->state |= QStyle::State_On;
    }
    if (!this->isDown())
    {
        option->state |= QStyle::State_Raised;
    }

    option->text = this->text();
    option->icon = this->icon();
    option->iconSize = QSize(this->style()->pixelMetric(QStyle::PM_IndicatorWidth, option, this),
                             this->style()->pixelMetric(QStyle::PM_IndicatorHeight, option, this));
    int buttonHeight = this->buttonSizeHint().height();
    option->rect.setHeight(buttonHeight);
}


CollapsibleButtonPrivate::CollapsibleButtonPrivate(CollapsibleButton &object)
    : q_ptr(&object)
{
    m_bForcingVisibility    = false;
    m_bStateCreated         = false;
}

void CollapsibleButtonPrivate::init()
{
    q_ptr->setCheckable(true);
    q_ptr->setChecked(true);
    m_bCollapsed    = false;
    m_collapsedHeight = 2;
    m_bExclusiveMouseOver   = false;
    m_bLookOffWhenChecked   = true;
    m_maximumHeight         = q_ptr->maximumHeight();
    m_textAlignment         = Qt::AlignLeft | Qt::AlignVCenter;
    m_indicatorAlignment    = Qt::AlignLeft;
    q_ptr->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Preferred));
    q_ptr->setContentsMargins(0, q_ptr->buttonSizeHint().height(), 0, 0);
    q_ptr->setBackgroundRole(QPalette::Window);

    QObject::connect(q_ptr, SIGNAL(toggled(bool)),
                     q_ptr, SLOT(onToggled(bool)));
}

void CollapsibleButtonPrivate::setChildVisibility(QWidget *childWidget)
{
    if (!q_ptr->testAttribute(Qt::WA_WState_Created))
    {
        return;
    }
    m_bForcingVisibility = true;

    bool visible= !m_bCollapsed;
    // if the widget has been explicity hidden, then hide it.
    if (childWidget->property("visibilityToParent").isValid()
            && !childWidget->property("visibilityToParent").toBool())
    {
        visible = false;
    }

    childWidget->setVisible(visible);

    // setVisible() has set the ExplicitShowHide flag, restore it as we don't want
    // to make it like it was an explicit visible set because we want
    // to allow the children to be explicitly hidden by the user.
    if ((!childWidget->property("visibilityToParent").isValid() ||
         childWidget->property("visibilityToParent").toBool()))
    {
        childWidget->setAttribute(Qt::WA_WState_ExplicitShowHide, false);
    }
    m_bForcingVisibility = false;
}

}
