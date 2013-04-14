#include "dockwindow.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QEvent>
#include <QPainter>
#include <QLabel>
#include <QToolButton>
#include <QSpacerItem>
#include <QAction>
namespace CustomUI{

namespace Internal{
class DockWindowTitlePrivate
{
public:
    explicit DockWindowTitlePrivate(DockWindowTitle *parent);
    QHBoxLayout *m_sysBtnLayout;
    QHBoxLayout *m_mainLayout;
    QToolButton *m_sysBtnClose;
    QToolButton *m_sysBtnMinMax;
    QLabel      *m_label;
    DockWindow  *m_dockWindow;
    QIcon m_iconLeftArrow;
    QIcon m_iconRightArrow;
};
DockWindowTitlePrivate::DockWindowTitlePrivate(DockWindowTitle *parent)
    : m_sysBtnLayout(new QHBoxLayout),
      m_mainLayout(new QHBoxLayout),
      m_sysBtnClose(new QToolButton(parent)),
      m_sysBtnMinMax(new QToolButton(parent)),
      m_label(new QLabel(parent)),
      m_iconLeftArrow(":/customui/media/lt.png"),
      m_iconRightArrow(":/customui/media/rt.png")

{

    m_sysBtnLayout->addWidget(m_sysBtnMinMax);
    m_sysBtnLayout->addWidget(m_sysBtnClose);

    m_sysBtnLayout->setContentsMargins(0, 0, 0, 0);

    QSpacerItem *spacerItem = new QSpacerItem(40, 10,
                                              QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_mainLayout->addWidget(m_label);
    m_label->setStyleSheet("color:white");
    m_mainLayout->addItem(spacerItem);
    m_mainLayout->addLayout(m_sysBtnLayout);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    QAction *closeAction = new QAction(parent);
    QAction *minMaxAction = new QAction(parent);
    m_sysBtnClose->setDefaultAction(closeAction);
    m_sysBtnClose->setStyleSheet("QToolButton:hover{image:url(:/customui/media/x_hover.png)}"
                                 "QToolButton:hover:pressed{image:url(:/customui/media/x_down.png)}"
                                 "QToolButton:!hover{image:url(:/customui/media/x_normal.png)}"
                                 );
    m_sysBtnMinMax->setDefaultAction(minMaxAction);
    m_sysBtnLayout->setSpacing(0);

}


DockWindowTitle::DockWindowTitle(QDockWidget *parent, SystemButtons btns)
    : QWidget(parent),
      d(new DockWindowTitlePrivate(this)),
      m_parent(parent),
      m_bHover(false),
      m_bMin(false)
{
    this->setStyleSheet("background-color: transparent");
    setMouseTracking(true);
    if(!(btns & DOCKBUTTON_CLOSE))
        d->m_sysBtnClose->hide();
    if(!(btns & DOCKBUTTON_MINMAX))
        d->m_sysBtnMinMax->hide();
    this->setLayout(d->m_mainLayout);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(d->m_sysBtnClose, SIGNAL(clicked()),
            this, SLOT(onClose()));
    connect(d->m_sysBtnMinMax, SIGNAL(clicked()),
            this, SLOT(onMinMaxClicked()));
    updateActions();
}

void DockWindowTitle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect r = rect();
    QLinearGradient lg(0, 0, 0, r.height());

    if (m_bHover)
    {
        lg.setColorAt(0, "#656565");
        lg.setColorAt(1, "#494949");
    }
    else
    {
        lg.setColorAt(0, "#5c5c5c");
        lg.setColorAt(1, "#3e3e3e");
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(lg);

    painter.drawRect(0, 1, r.width(), r.height());
    painter.setPen("#000000");
    painter.drawLine(0, r.height() - 1, r.width() - 1, r.height() - 1);

    painter.setPen("#e3e3e3");
}

void DockWindowTitle::mousePressEvent(QMouseEvent *e)
{
    e->ignore();
}

void DockWindowTitle::mouseMoveEvent(QMouseEvent *e)
{
    e->ignore();
}

void DockWindowTitle::enterEvent(QEvent *e)
{
    e->ignore();
    m_bHover = true;
    update();
}

void DockWindowTitle::leaveEvent(QEvent *e)
{
    e->ignore();
    m_bHover = false;
    update();
}

QSize DockWindowTitle::sizeHint() const
{
    ensurePolished();
    return d->m_mainLayout->sizeHint();
}

QSize DockWindowTitle::minimumSizeHint() const
{
    return d->m_mainLayout->minimumSize();
}

void DockWindowTitle::updateActions()
{
    if(m_bMin)
    {
        d->m_sysBtnMinMax->setIcon(d->m_iconLeftArrow);
    }
    else
    {
        d->m_sysBtnMinMax->setIcon(d->m_iconRightArrow);
    }
    if(m_parent)
        d->m_sysBtnClose->setVisible(m_parent->isFloating());
}

void DockWindowTitle::onClose()
{
    emit closeClicked();
}

void DockWindowTitle::onMinMaxClicked()
{
    m_bMin = !m_bMin;
    emit minMaxClicked(m_bMin);
    updateActions();
}

void DockWindowTitle::setTitle(const QString &str)
{
    d->m_label->setText(str);
}


}//namespace Internal
using namespace Internal;
DockWindow::DockWindow(const QString &title, QWidget *parent) :
    QDockWidget(title, parent),
    m_widget(new QWidget(this))
{
    this->setObjectName(title);
    this->setWidget(m_widget);
    m_titleBar = new DockWindowTitle(this,
                                     DockWindowTitle::DOCKBUTTON_MINMAX |
                                     DockWindowTitle::DOCKBUTTON_CLOSE);
    m_titleBar->setTitle(title);
    this->setTitleBarWidget(m_titleBar);
    connect(m_titleBar, SIGNAL(closeClicked()),
            this, SLOT(onClosed()));
    connect(m_titleBar, SIGNAL(minMaxClicked(bool&)),
            this, SLOT(onMinMax(bool&)));
    connect(this, SIGNAL(topLevelChanged(bool)),
            this, SLOT(onTopLevelChanged(bool)));

}

DockWindow::~DockWindow()
{
}

void DockWindow::onClosed()
{
    emit closeClicked();
}

void DockWindow::onMinMax(bool &bMin)
{
    emit minMaxClicked(bMin);
}

void DockWindow::onTopLevelChanged(bool topLevel)
{
    m_titleBar->updateActions();
}
}
