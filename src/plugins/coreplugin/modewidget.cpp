#include "modewidget.h"
#include "utils/stylehelper.h"

#include <QEvent>
#include <QToolTip>
#include <QPainter>
#include <QMouseEvent>
#include <QFont>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QFontMetrics>
#include <QStatusBar>
#include <QStackedLayout>
#include <QPalette>
#include <QPushButton>
#include <QToolBar>
namespace Core{
namespace Internal{

const int ModeTabBar::m_rounding = 22;
const int ModeTabBar::m_textPadding = 4;

void ModeTab::fadeIn()
{
    animator.stop();
    animator.setDuration(80);
    animator.setEndValue(40);
    animator.start();
}

void ModeTab::fadeOut()
{
    animator.stop();
    animator.setDuration(160);
    animator.setEndValue(0);
    animator.start();
}

void ModeTab::setFader(float value)
{
    m_fader = value;
    tabbar->update();
}

ModeTabBar::ModeTabBar(QWidget *parent)
    :QWidget(parent)
{
    m_hoverIndex = -1;
    m_currentIndex = -1;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setMinimumWidth(qMax(2 * m_rounding, 40));
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    m_triggerTimer.setSingleShot(true);

    connect(&m_triggerTimer, SIGNAL(timeout()), this, SLOT(emitCurrentIndex()));

}

ModeTabBar::~ModeTabBar()
{
    delete style();
}

bool ModeTabBar::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        if (validIndex(m_hoverIndex))
        {
            QString tt = tabToolTip(m_hoverIndex);
            if (!tt.isEmpty())
            {
                QToolTip::showText(static_cast<QHelpEvent*>(event)->globalPos(), tt, this);
                return true;
            }
        }
    }
    return QWidget::event(event);
}

void ModeTabBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter p(this);

    for (int i = 0; i < count(); ++i)
        if (i != currentIndex())
            paintTab(&p, i);

    if (currentIndex() != -1)
        paintTab(&p, currentIndex());
}

void ModeTabBar::paintTab(QPainter *painter, int tabIndex) const
{
    //检查索引有效性
    if (!validIndex(tabIndex))
    {
        qWarning("invalid index");
        return;
    }
    painter->save();

    QRect rect = tabRect(tabIndex);
    bool selected = (tabIndex == m_currentIndex);
    bool enabled = isTabEnabled(tabIndex);

    //
    if (selected)
    {
        //background
        painter->save();
        QLinearGradient grad(rect.topLeft(), rect.topRight());
        grad.setColorAt(0, QColor(255, 255, 255, 140));
        grad.setColorAt(1, QColor(255, 255, 255, 210));
        painter->fillRect(rect.adjusted(0, 0, 0, -1), grad);
        

        //shadows
        painter->setPen(QColor(0, 0, 0, 110));
        painter->drawLine(rect.topLeft() + QPoint(1,-1), rect.topRight() - QPoint(0,1));
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        painter->setPen(QColor(0, 0, 0, 40));
        painter->drawLine(rect.topLeft(), rect.bottomLeft());

        //highlights
        painter->setPen(QColor(255, 255, 255, 50));
        painter->drawLine(rect.topLeft() + QPoint(0, -2), rect.topRight() - QPoint(0,2));
        painter->drawLine(rect.bottomLeft() + QPoint(0, 1), rect.bottomRight() + QPoint(0,1));
        painter->setPen(QColor(255, 255, 255, 40));
        painter->drawLine(rect.topLeft() + QPoint(0, 0), rect.topRight());
        painter->drawLine(rect.topRight() + QPoint(0, 1), rect.bottomRight() - QPoint(0, 1));
        painter->drawLine(rect.bottomLeft() + QPoint(0,-1), rect.bottomRight()-QPoint(0,1));
		painter->restore();
    }

    QString tabText(this->tabText(tabIndex));
    QRect tabTextRect(tabRect(tabIndex));
    QRect tabIconRect(tabTextRect);
    tabTextRect.translate(0, -2);
    QFont boldFont(painter->font());
    boldFont.setPointSizeF(Utils::StyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    painter->setFont(boldFont);
    painter->setPen(selected ? QColor(255, 255, 255, 160) : QColor(0, 0, 0, 110));
    int textFlags = Qt::AlignCenter | Qt::AlignBottom | Qt::TextWordWrap;
    if (enabled)
    {
        painter->drawText(tabTextRect, textFlags, tabText);
        painter->setPen(selected ? QColor(60, 60, 60) : Utils::StyleHelper::panelTextColor());
    }
    else
    {
        painter->setPen(selected ? Utils::StyleHelper::panelTextColor() : QColor(255, 255, 255, 120));
    }
#ifndef Q_OS_MAC
    if (!selected && enabled)
    {
        painter->save();
        int fader = int(m_tabs[tabIndex]->fader());
        QLinearGradient grad(rect.topLeft(), rect.topRight());
        grad.setColorAt(0, Qt::transparent);
        grad.setColorAt(0.5, QColor(255, 255, 255, fader));
        grad.setColorAt(1, Qt::transparent);
        painter->fillRect(rect, grad);
        painter->setPen(QPen(grad, 1.0));
        painter->drawLine(rect.topLeft(), rect.topRight());
        painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        painter->restore();
    }
#endif

    if (!enabled)
        painter->setOpacity(0.7);

    int textHeight = painter->fontMetrics().boundingRect(QRect(0, 0, width(), height()), Qt::TextWordWrap, tabText).height();
    tabIconRect.adjust(0, 4, 0, -textHeight);
   Utils::StyleHelper::drawIconWithShadow(tabIcon(tabIndex), tabIconRect, painter, enabled ? QIcon::Normal : QIcon::Disabled);

    painter->translate(0, -1);
    painter->drawText(tabTextRect, textFlags, tabText);
    painter->restore();
}

void ModeTabBar::mousePressEvent(QMouseEvent *e)
{
    e->accept();
    for (int index = 0; index < m_tabs.count(); ++index)
    {
        if (tabRect(index).contains(e->pos()))
        {
            if (isTabEnabled(index))
            {
                m_currentIndex = index;
                update();
                m_triggerTimer.start(0);
            }
            break;
        }
    }
}

void ModeTabBar::mouseMoveEvent(QMouseEvent *e)
{
    int newHover = -1;
    for (int i = 0; i < count(); ++i)
    {
        QRect area = tabRect(i);
        if (area.contains(e->pos()))
        {
            newHover = i;
            break;
        }
    }
    if (newHover == m_hoverIndex)
        return;

    if (validIndex(m_hoverIndex))
        m_tabs[m_hoverIndex]->fadeOut();

    m_hoverIndex = newHover;

    if (validIndex(m_hoverIndex))
    {
        m_tabs[m_hoverIndex]->fadeIn();
        m_hoverRect = tabRect(m_hoverIndex);
    }
}

void ModeTabBar::enterEvent(QEvent *e)
{
    Q_UNUSED(e)
    m_hoverRect = QRect();
    m_hoverIndex = -1;
}

void ModeTabBar::leaveEvent(QEvent * e)
{
    Q_UNUSED(e)
    m_hoverIndex = -1;
    m_hoverRect = QRect();
    for (int i = 0 ; i < m_tabs.count() ; ++i)
    {
        m_tabs[i]->fadeOut();
    }
}

QSize ModeTabBar::sizeHint() const
{
    QSize sh = tabSizeHint();
    return QSize(sh.width() * m_tabs.count(), sh.height() );
}

QSize ModeTabBar::minimumSizeHint() const
{
    QSize sh = tabSizeHint(true);
    return QSize(sh.width() * m_tabs.count(), sh.height());
}

void ModeTabBar::setTabEnabled(int index, bool enable)
{
    Q_ASSERT(index < m_tabs.size());
    Q_ASSERT(index >= 0);

    if (index < m_tabs.size() && index >= 0)
    {
        m_tabs[index]->enabled = enable;
        update(tabRect(index));
    }
}

bool ModeTabBar::isTabEnabled(int index) const
{
    Q_ASSERT(index < m_tabs.size());
    Q_ASSERT(index >= 0);

    if (index < m_tabs.size() && index >= 0)
        return m_tabs[index]->enabled;

    return false;
}


void ModeTabBar::setCurrentIndex(int index)
{
    if (isTabEnabled(index))
    {
        m_currentIndex = index;
        update();
        emit currentChanged(m_currentIndex);
    }
}

QRect ModeTabBar::tabRect(int index) const
{
    QSize sh = tabSizeHint();

    if (sh.width() * m_tabs.count() > width())
        sh.setWidth(width() / m_tabs.count());

    return QRect(index * sh.width(), 0, sh.width(), sh.height());
}

void ModeTabBar::emitCurrentIndex()
{
    emit currentChanged(m_currentIndex);
}

QSize ModeTabBar::tabSizeHint(bool minimum) const
{
    QFont boldFont(font());
    boldFont.setPointSizeF(Utils::StyleHelper::sidebarFontSize());
    boldFont.setBold(true);
    QFontMetrics fm(boldFont);
    int spacing = 8;
    int width = 60 + spacing + 2;
    int maxLabelwidth = 0;
    for (int tab=0 ; tab<count() ;++tab)
    {
        int width = fm.width(tabText(tab));
        if (width > maxLabelwidth)
            maxLabelwidth = width;
    }
    int iconHeight = minimum ? 0 : 32;
    return QSize(qMax(width, maxLabelwidth + 4),
                 iconHeight + spacing + fm.height());

}

ModeTabWidget::ModeTabWidget(QWidget *parent) :
    QWidget(parent)
{
    m_tabBar = new ModeTabBar(this);
    m_modeToolBar = new QWidget(this);
    m_modeToolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_toolBarLayout = new QHBoxLayout(m_modeToolBar);
    m_toolBarLayout->setContentsMargins(1, 1, 1, 1);
    m_toolBarLayout->setSpacing(2);

    m_selectionWidget = new QWidget(this);
    QHBoxLayout *selectionLayout = new QHBoxLayout;
    selectionLayout->setSpacing(0);
    selectionLayout->setMargin(0);
    selectionLayout->addSpacerItem(new QSpacerItem(100, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));
    selectionLayout->addWidget(m_modeToolBar);
    selectionLayout->addWidget(m_tabBar);

    m_selectionWidget->setLayout(selectionLayout);
    m_selectionWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_modesStack = new QStackedLayout;
    m_statusBar = new QStatusBar;
    m_statusBar->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setMargin(0);
    vlayout->setSpacing(0);
    vlayout->addLayout(m_modesStack);
    vlayout->addWidget(m_statusBar);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(1);

    mainLayout->addWidget(m_selectionWidget);
    mainLayout->addLayout(vlayout);

    setLayout(mainLayout);

    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(showWidget(int)));

}

void ModeTabWidget::insertTab(int index, QWidget *tab, const QIcon &icon, const QString &label)
{
    m_modesStack->insertWidget(index, tab);
    m_tabBar->insertTab(index, icon, label);
}

void ModeTabWidget::removeTab(int index)
{
    m_modesStack->removeWidget(m_modesStack->widget(index));
    m_tabBar->removeTab(index);
}

void ModeTabWidget::setBackgroundBrush(const QBrush &brush)
{
    QPalette pal = m_tabBar->palette();
    pal.setBrush(QPalette::Mid, brush);
    m_tabBar->setPalette(pal);
}

void ModeTabWidget::setTabToolTip(int index, const QString &toolTip)
{
    m_tabBar->setTabToolTip(index, toolTip);
}

void ModeTabWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    QRect rect = m_selectionWidget->rect().adjusted(0, 0, 1, 0);
    rect = style()->visualRect(layoutDirection(), geometry(), rect);
    Utils::StyleHelper::horizontalGradient(&painter, rect, rect);
    painter.setPen(Utils::StyleHelper::borderColor());
    painter.drawLine(rect.topRight(), rect.bottomRight());

    QColor light = Utils::StyleHelper::sidebarHighlight();
    painter.setPen(light);
    painter.drawLine(rect.bottomLeft(), rect.bottomRight());
}

int ModeTabWidget::currentIndex() const
{
    return m_tabBar->currentIndex();
}

QStatusBar *ModeTabWidget::statusBar() const
{
    return m_statusBar;
}

void ModeTabWidget::setTabEnabled(int index, bool enable)
{
    m_tabBar->setTabEnabled(index, enable);
}

bool ModeTabWidget::isTabEnabled(int index) const
{
    return m_tabBar->isTabEnabled(index);
}

void ModeTabWidget::addToolBar(QToolBar *toolBar)
{
    if(!toolBar ||m_toolBars.contains(toolBar))
        return ;

    toolBar->setStyleSheet("border:none");
    if(toolBar->windowTitle().isEmpty())
    {
        toolBar->setWindowTitle(tr("ToolBar %1").arg(m_toolBars.count() + 1));
    }
    m_toolBars.append(toolBar);
    m_toolBarLayout->addWidget(toolBar);
}

void ModeTabWidget::removeToolBar(QToolBar *toolBar)
{
    m_toolBars.removeAll(toolBar);
    m_toolBarLayout->removeWidget(toolBar);
}

void ModeTabWidget::setCurrentIndex(int index)
{
    if (m_tabBar->isTabEnabled(index))
        m_tabBar->setCurrentIndex(index);
}

void ModeTabWidget::setSelectionWidgetHidden(bool hidden)
{
    m_selectionWidget->setHidden(hidden);
}

void ModeTabWidget::showWidget(int index)
{
    emit currentAboutToShow(index);
    m_modesStack->setCurrentIndex(index);
    emit currentChanged(index);
}

void ModeTabWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createPopupMenu();
    menu->exec(event->globalPos());
    delete menu;
    event->accept();
}

QMenu *ModeTabWidget::createPopupMenu()
{
    QMenu *menu = new QMenu(this);
    foreach(QToolBar *toolBar, m_toolBars)
    {
        QAction *action = new QAction(this);
        action->setText(toolBar->windowTitle());
        menu->addAction(action);
    }
    return menu;
}

}//Internal

}//Core
