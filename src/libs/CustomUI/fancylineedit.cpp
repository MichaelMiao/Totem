#include "fancylineedit.h"
#include "iconbutton.h"

#include <QEvent>
#include <QMenu>
#include <QStyle>

namespace CustomUI{

class FancyLineEditPrivate : public QObject
{
public:
    explicit FancyLineEditPrivate(FancyLineEdit *parent);

    virtual bool eventFilter(QObject *obj, QEvent *event);

    FancyLineEdit  *m_lineEdit;
    QPixmap m_pixmap[2];
    QMenu *m_menu[2];
    bool m_menuTabFocusTrigger[2];
    IconButton *m_iconbutton[2];
    bool m_iconEnabled[2];
};

FancyLineEditPrivate::FancyLineEditPrivate(FancyLineEdit *parent)
    : QObject(parent),
      m_lineEdit(parent)
{
    for (int i = 0; i < 2; ++i)
    {
        m_menu[i] = 0;
        m_menuTabFocusTrigger[i] = false;
        m_iconbutton[i] = new IconButton(parent);
        m_iconbutton[i]->installEventFilter(this);
        m_iconbutton[i]->hide();
        m_iconbutton[i]->setAutoHide(false);
        m_iconEnabled[i] = false;
    }
}

bool FancyLineEditPrivate::eventFilter(QObject *obj, QEvent *event)
{
    int buttonIndex = -1;
    for (int i = 0; i < 2; ++i)
    {
        if (obj == m_iconbutton[i])
        {
            buttonIndex = i;
            break;
        }
    }
    if (buttonIndex == -1)
        return QObject::eventFilter(obj, event);
    switch (event->type())
    {
    case QEvent::FocusIn:
        if (m_menuTabFocusTrigger[buttonIndex] && m_menu[buttonIndex])
        {
            m_lineEdit->setFocus();
            m_menu[buttonIndex]->exec(m_iconbutton[buttonIndex]->mapToGlobal(
                    m_iconbutton[buttonIndex]->rect().center()));
            return true;
        }
    default:
        break;
    }
    return QObject::eventFilter(obj, event);
}

FancyLineEdit::FancyLineEdit(QWidget *parent) :
    QLineEdit(parent),
    d(new FancyLineEditPrivate(this))
{
    ensurePolished();
    updateMargins();

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(checkButtons(QString)));
    connect(d->m_iconbutton[Left], SIGNAL(clicked()), this, SLOT(iconClicked()));
    connect(d->m_iconbutton[Right], SIGNAL(clicked()), this, SLOT(iconClicked()));

}

FancyLineEdit::~FancyLineEdit()
{

}

QPixmap FancyLineEdit::buttonPixmap(FancyLineEdit::Side side) const
{
    return d->m_pixmap[side];
}

void FancyLineEdit::setButtonPixmap(FancyLineEdit::Side side, const QPixmap &pixmap)
{
    d->m_iconbutton[side]->setPixmap(pixmap);
    updateMargins();
    updateButtonPositions();
    update();
}

QMenu *FancyLineEdit::buttonMenu(FancyLineEdit::Side side) const
{
    return  d->m_menu[side];
}

void FancyLineEdit::setButtonMenu(FancyLineEdit::Side side, QMenu *buttonMenu)
{
    d->m_menu[side] = buttonMenu;
    d->m_iconbutton[side]->setIconOpacity(1.0);
}

void FancyLineEdit::setButtonVisible(FancyLineEdit::Side side, bool visible)
{
    d->m_iconbutton[side]->setVisible(visible);
    d->m_iconEnabled[side] = visible;
    updateMargins();
}

bool FancyLineEdit::isButtonVisible(FancyLineEdit::Side side) const
{
    return d->m_iconEnabled[side];
}

void FancyLineEdit::setButtonToolTip(FancyLineEdit::Side side, const QString &tip)
{
    d->m_iconbutton[side]->setToolTip(tip);
}

void FancyLineEdit::setButtonFocusPolicy(FancyLineEdit::Side side, Qt::FocusPolicy policy)
{
    d->m_iconbutton[side]->setFocusPolicy(policy);
}

void FancyLineEdit::setMenuTabFocusTrigger(FancyLineEdit::Side side, bool v)
{
    if (d->m_menuTabFocusTrigger[side] == v)
        return;

    d->m_menuTabFocusTrigger[side] = v;
    d->m_iconbutton[side]->setFocusPolicy(v ? Qt::TabFocus : Qt::NoFocus);

}

bool FancyLineEdit::hasMenuTabFocusTrigger(FancyLineEdit::Side side) const
{
    return d->m_menuTabFocusTrigger[side];
}

void FancyLineEdit::setAutoHideButton(FancyLineEdit::Side side, bool h)
{
    d->m_iconbutton[side]->setAutoHide(h);
    if (h)
        d->m_iconbutton[side]->setIconOpacity(text().isEmpty() ?  0.0 : 1.0);
    else
        d->m_iconbutton[side]->setIconOpacity(1.0);

}

bool FancyLineEdit::hasAutoHideButton(FancyLineEdit::Side side) const
{
    return d->m_iconbutton[side]->hasAutoHide();
}

void FancyLineEdit::checkButtons(const QString &text)
{
    if (m_oldText.isEmpty() || text.isEmpty())
    {
        for (int i = 0; i < 2; ++i)
        {
            if (d->m_iconbutton[i]->hasAutoHide())
                d->m_iconbutton[i]->animateShow(!text.isEmpty());
        }
        m_oldText = text;
    }
}

void FancyLineEdit::iconClicked()
{
    IconButton *button = qobject_cast<IconButton *>(sender());
    int index = -1;
    for (int i = 0; i < 2; ++i)
        if (d->m_iconbutton[i] == button)
            index = i;
    if (index == -1)
        return;
    if (d->m_menu[index])
    {
        d->m_menu[index]->exec(QCursor::pos());
    }
    else
    {
        emit buttonClicked((Side)index);
        if (index == Left)
            emit leftButtonClicked();
        else if (index == Right)
            emit rightButtonClicked();
    }
}

void FancyLineEdit::resizeEvent(QResizeEvent *e)
{
    updateButtonPositions();
}

void FancyLineEdit::updateMargins()
{
    bool leftToRight = (layoutDirection() == Qt::LeftToRight);
    Side realLeft = (leftToRight ? Left : Right);
    Side realRight = (leftToRight ? Right : Left);

    int leftMargin = d->m_iconbutton[realLeft]->pixmap().width() + 8;
    int rightMargin = d->m_iconbutton[realRight]->pixmap().width() + 8;
    //用于KDE中
    if (style()->inherits("OxygenStyle"))
    {
        leftMargin = qMax(24, leftMargin);
        rightMargin = qMax(24, rightMargin);
    }
    //设置文字的margin
    QMargins margins((d->m_iconEnabled[realLeft] ? leftMargin : 0), 0,
                     (d->m_iconEnabled[realRight] ? rightMargin : 0), 0);

    setTextMargins(margins);
}

void FancyLineEdit::updateButtonPositions()
{
    QRect contentRect = rect();
    for (int i = 0; i < 2; ++i)
    {
        Side iconpos = (Side)i;
        if (layoutDirection() == Qt::RightToLeft)
            iconpos = (iconpos == Left ? Right : Left);

        if (iconpos == FancyLineEdit::Right)
        {
            const int iconoffset = textMargins().right() + 4;
            d->m_iconbutton[i]->setGeometry(contentRect.adjusted(width() - iconoffset, 0, 0, 0));
        }
        else
        {
            const int iconoffset = textMargins().left() + 4;
            d->m_iconbutton[i]->setGeometry(contentRect.adjusted(0, 0, -width() + iconoffset, 0));
        }
    }
}
}



void AAA::func()
{}

CUSTOMUI_EXPORT int testfunc()
{
	return 100;
}

