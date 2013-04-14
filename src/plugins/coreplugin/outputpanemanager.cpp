#include "outputpanemanager.h"
#include "coreconstants.h"
#include "ioutputpane.h"
#include "outputpaneplaceholder.h"
#include "actionmanager/actioncontainer.h"
#include "actionmanager/actionmanager.h"
#include "extensionsystem/pluginmanager.h"
#include "utils/totemassert.h"
#include "icore.h"

#include <QAction>
#include <QApplication>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPainter>
#include <QSplitter>
#include <QStyle>
#include <QStackedWidget>
#include <QToolButton>
#include <QTimeLine>
#include <QLabel>
#include <QPushButton>
#include <QStyleOption>

using namespace Core;
using namespace Core::Internal;

static OutputPaneManager *m_instance = 0;

static char outputPaneSettingsKeyC[] = "OutputPaneVisibility";
static char outputPaneIdKeyC[] = "id";
static char outputPaneVisibleKeyC[] = "visible";

bool comparePanes(IOutputPane *p1, IOutputPane *p2)
{
    return p1->priorityInStatusBar() > p2->priorityInStatusBar();
}

static inline int paneShortCut(int number)
{
#ifdef Q_OS_MAC
    int modifier = Qt::CTRL;
#else
    int modifier = Qt::ALT;
#endif
    return modifier | (Qt::Key_0 + number);
}

OutputPaneManager::OutputPaneManager(QWidget *parent) :
    QWidget(parent),
    m_titleLabel(new QLabel),
    m_closeButton(new QToolButton),
    m_minMaxAction(0),
    m_manageButton(new OutputPaneManageButton),
    m_minMaxButton(new QToolButton),
    m_outputWidgetPane(new QStackedWidget),
    m_opToolBarWidgets(new QStackedWidget),
    m_minimizeIcon(QLatin1String(":/core/images/go-down.png")),
    m_maximizeIcon(QLatin1String(":/core/images/go-up.png")),
    m_maximised(false),
    m_lastIndex(0)
{
    setObjectName(QLatin1String("OutputPaneManager"));
    m_clearAction = new QAction(this);
    m_clearAction->setIcon(QIcon(QLatin1String(Constants::ICON_CLEAN_PANE)));
    m_clearAction->setText(tr("Clear"));
    connect(m_clearAction, SIGNAL(triggered()), this, SLOT(clearPage()));

    m_minMaxAction = new QAction(this);
    m_minMaxAction->setIcon(m_maximizeIcon);
    m_minMaxAction->setText(tr("Maximize Output Pane"));

    m_closeButton->setIcon(QIcon(QLatin1String(Constants::ICON_CLOSE_DOCUMENT)));
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(slotHide()));
    QVBoxLayout *mainlayout = new QVBoxLayout;
    mainlayout->setSpacing(0);
    mainlayout->setMargin(0);
    m_toolBar = new QWidget;
    QHBoxLayout *toolLayout = new QHBoxLayout(m_toolBar);
    toolLayout->setMargin(0);
    toolLayout->setSpacing(0);

    m_clearButton = new QToolButton;
    toolLayout->addWidget(m_clearButton);

    toolLayout->addWidget(m_opToolBarWidgets);

    toolLayout->addWidget(m_minMaxButton);

    toolLayout->addWidget(m_closeButton);
    mainlayout->addWidget(m_toolBar);
    mainlayout->addWidget(m_outputWidgetPane, 10);
    setLayout(mainlayout);

    m_buttonsWidget = new QWidget;
    m_buttonsWidget->setLayout(new QHBoxLayout);
    m_buttonsWidget->layout()->setContentsMargins(5,0,0,0);
    m_buttonsWidget->layout()->setSpacing(4);

    m_buttonsWidget->layout()->addWidget(m_manageButton);
    connect(m_manageButton, SIGNAL(clicked()), this, SLOT(popupMenu()));

}

OutputPaneManager::~OutputPaneManager()
{

}

void OutputPaneManager::showPage(bool focus, bool ensureSizeHint)
{
    int idx = findIndexForPage(qobject_cast<IOutputPane*>(sender()));
    showPage(idx, focus);
    if (ensureSizeHint && OutputPanePlaceHolder::getCurrent())
        OutputPanePlaceHolder::getCurrent()->ensureSizeHintAsMinimum();
}

void OutputPaneManager::togglePage(bool focus)
{
    int idx = findIndexForPage(qobject_cast<IOutputPane*>(sender()));
    if (OutputPanePlaceHolder::isCurrentVisible()
            && currentIndex() == idx)
         slotHide();
    else
         showPage(idx, focus);
}

void OutputPaneManager::showPage(int idx, bool focus)
{
    TOTEM_ASSERT(idx >= 0, return);
    if (OutputPanePlaceHolder *ph = OutputPanePlaceHolder::getCurrent())
    {
        ph->setVisible(true);
        ensurePageVisible(idx);
        IOutputPane *out = m_panes.at(idx);
        out->visibilityChanged(true);
        if (focus && out->canFocus())
            out->setFocus();
    }
}

void OutputPaneManager::ensurePageVisible(int idx)
{
    setCurrentIndex(idx);
}

void OutputPaneManager::setCurrentIndex(int idx)
{
    static int lastIndex = -1;

    if (lastIndex != -1)
    {
        m_buttons.at(lastIndex)->setChecked(false);
        m_panes.at(lastIndex)->visibilityChanged(false);
    }

    if (idx != -1)
    {
        m_outputWidgetPane->setCurrentIndex(idx);
        m_opToolBarWidgets->setCurrentIndex(idx);

        IOutputPane *pane = m_panes.at(idx);
        pane->visibilityChanged(true);
        m_buttons.at(idx)->setChecked(OutputPanePlaceHolder::isCurrentVisible());
        m_titleLabel->setText(pane->displayName());
    }

    lastIndex = idx;
}

void OutputPaneManager::readSettings()
{
    QMap<QString, bool> visibility;
    QSettings *settings = ICore::settings();
    int num = settings->beginReadArray(QLatin1String(outputPaneSettingsKeyC));
    for (int i = 0; i < num; ++i)
    {
        settings->setArrayIndex(i);
        visibility.insert(settings->value(QLatin1String(outputPaneIdKeyC)).toString(),
                          settings->value(QLatin1String(outputPaneVisibleKeyC)).toBool());
    }
    settings->endArray();

    for (int i = 0; i < m_ids.size(); ++i)
    {
        if (visibility.contains(m_ids.at(i).toString()))
            m_buttons.at(i)->setVisible(visibility.value(m_ids.at(i).toString()));
    }
}

void OutputPaneManager::saveSettings()
{
    QSettings *settings = ICore::settings();
    settings->beginWriteArray(QLatin1String(outputPaneSettingsKeyC),
                              m_ids.size());
    for (int i = 0; i < m_ids.size(); ++i)
    {
        settings->setArrayIndex(i);
        settings->setValue(QLatin1String(outputPaneIdKeyC), m_ids.at(i).toString());
        settings->setValue(QLatin1String(outputPaneVisibleKeyC), m_buttons.at(i)->isVisible());
    }
    settings->endArray();
}

int OutputPaneManager::currentIndex() const
{
    return m_outputWidgetPane->currentIndex();
}

int OutputPaneManager::findIndexForPage(IOutputPane *out)
{
    return m_panes.indexOf(out);
}

void OutputPaneManager::buttonTriggered(int idx)
{
    TOTEM_ASSERT(idx >= 0, return);
    if (idx == currentIndex()
            && OutputPanePlaceHolder::isCurrentVisible())
    {
        slotHide();
    }
    else
    {
        showPage(idx, true);
    }
}

void OutputPaneManager::init()
{
    ActionManager *am = ActionManager::instance();
    ActionContainer *mwindow = am->actionContainer(Constants::M_WINDOW);
    const Context globalContext(Constants::C_GLOBAL);

    // Window->Output Panes
    ActionContainer *mpanes = am->createMenu(Constants::M_WINDOW_PANES);
    mwindow->addMenu(mpanes, Constants::G_WINDOW_PANES);
    mpanes->menu()->setTitle(tr("Output &Panes"));
    mpanes->appendGroup("Core.OutputPane.ActionsGroup");
    mpanes->appendGroup("Core.OutputPane.PanesGroup");

    Command *cmd;

    cmd = am->registerAction(m_clearAction, "Core.OutputPane.clear", globalContext);
    m_clearButton->setDefaultAction(cmd->action());
    mpanes->addAction(cmd, "Core.OutputPane.ActionsGroup");

    cmd = am->registerAction(m_minMaxAction, "Core.OutputPane.minmax", globalContext);
    cmd->setDefaultKeySequence(QKeySequence(tr("Alt+9")));
    cmd->setAttribute(Command::CA_UpdateText);
    cmd->setAttribute(Command::CA_UpdateIcon);
    mpanes->addAction(cmd, "Core.OutputPane.ActionsGroup");
    connect(m_minMaxAction, SIGNAL(triggered()), this, SLOT(slotMinMax()));
    m_minMaxButton->setDefaultAction(cmd->action());

    mpanes->addSeparator(globalContext, "Core.OutputPane.ActionsGroup");

    QFontMetrics titleFm = m_titleLabel->fontMetrics();
    int minTitleWidth = 0;

    m_panes = ExtensionSystem::PluginManager::instance()->getObjects<IOutputPane>();
    qSort(m_panes.begin(), m_panes.end(), &comparePanes);
    const int n = m_panes.size();

    int shortcutNumber = 1;
    for (int i = 0; i != n; ++i)
    {
        IOutputPane *outPane = m_panes.at(i);
        const int idx = m_outputWidgetPane->addWidget(outPane->outputWidget(0));
        TOTEM_CHECK(idx == i);

        connect(outPane, SIGNAL(showPage(bool,bool)), this, SLOT(showPage(bool,bool)));
        connect(outPane, SIGNAL(hidePage()), this, SLOT(slotHide()));
        connect(outPane, SIGNAL(togglePage(bool)), this, SLOT(togglePage(bool)));

        QWidget *toolButtonsContainer = new QWidget(m_opToolBarWidgets);
        QHBoxLayout *toolButtonsLayout = new QHBoxLayout;
        toolButtonsLayout->setMargin(0);
        toolButtonsLayout->setSpacing(0);
        foreach (QWidget *toolButton, outPane->toolBarWidgets())
            toolButtonsLayout->addWidget(toolButton);
        toolButtonsLayout->addStretch(5);
        toolButtonsContainer->setLayout(toolButtonsLayout);

        m_opToolBarWidgets->addWidget(toolButtonsContainer);

        minTitleWidth = qMax(minTitleWidth, titleFm.width(outPane->displayName()));

        QString actionId = QLatin1String("Totem.Pane.") + outPane->displayName().simplified();
        actionId.remove(QLatin1Char(' '));
        Id id(actionId);
        QAction *action = new QAction(outPane->displayName(), this);
        Command *cmd = am->registerAction(action, id, globalContext);

        mpanes->addAction(cmd, "Core.OutputPane.PanesGroup");
        m_actions.append(cmd->action());
        m_ids.append(id);

        cmd->setDefaultKeySequence(QKeySequence(paneShortCut(shortcutNumber)));
        OutputPaneToggleButton *button =
                new OutputPaneToggleButton(
                    shortcutNumber,
                    outPane->displayName(),
                    cmd->action());
        ++shortcutNumber;
        m_buttonsWidget->layout()->addWidget(button);
        m_buttons.append(button);
        connect(button, SIGNAL(clicked()), this, SLOT(buttonTriggered()));

        bool visible = outPane->priorityInStatusBar() != -1;
        button->setVisible(visible);

        connect(cmd->action(), SIGNAL(triggered()), this, SLOT(shortcutTriggered()));
    }

    m_titleLabel->setMinimumWidth(minTitleWidth + m_titleLabel->contentsMargins().left()
                                  + m_titleLabel->contentsMargins().right());

    readSettings();
}

OutputPaneManager *OutputPaneManager::instance()
{
    return m_instance;
}

QWidget *OutputPaneManager::buttonsWidget()
{
    return m_buttonsWidget;
}

void OutputPaneManager::updateStatusButtons(bool visible)
{
    int idx = currentIndex();
    if (idx == -1)
        return;
    TOTEM_ASSERT(m_panes.size() == m_buttons.size(), return);
    m_buttons.at(idx)->setChecked(visible);
    m_panes.at(idx)->visibilityChanged(visible);
    OutputPanePlaceHolder *ph = OutputPanePlaceHolder::getCurrent();
    m_minMaxAction->setVisible(ph && ph->canMaximizeOrMinimize());
}

bool OutputPaneManager::isMaximized() const
{
    return m_maximised;
}

void OutputPaneManager::create()
{
    m_instance = new OutputPaneManager;
}

void OutputPaneManager::destroy()
{
    delete m_instance;
    m_instance = 0;
}

void OutputPaneManager::clearPage()
{
    int idx = currentIndex();
    if (idx >= 0)
        m_panes.at(idx)->clearContents();
}

void OutputPaneManager::buttonTriggered()
{
    OutputPaneToggleButton *button = qobject_cast<OutputPaneToggleButton *>(sender());
    buttonTriggered(m_buttons.indexOf(button));
}

void OutputPaneManager::popupMenu()
{
    QMenu menu;
    int idx = 0;
    foreach (IOutputPane *pane, m_panes) {
        QAction *act = menu.addAction(pane->displayName());
        act->setCheckable(true);
        act->setChecked(m_buttons.at(idx)->isVisible());
        act->setData(idx);
        ++idx;
    }
    QAction *result = menu.exec(QCursor::pos());
    if (!result)
        return;
    idx = result->data().toInt();
    TOTEM_ASSERT(idx >= 0 && idx < m_buttons.size(), return);
    QToolButton *button = m_buttons.at(idx);
    if (button->isVisible()) {
        m_panes.value(idx)->visibilityChanged(false);
        button->setChecked(false);
        button->hide();
    } else {
        button->show();
        showPage(idx, false);
    }
}


void OutputPaneManager::slotHide()
{
    OutputPanePlaceHolder *ph = OutputPanePlaceHolder::getCurrent();
    if (ph)
    {
        ph->setVisible(false);
        int idx = currentIndex();
        TOTEM_ASSERT(idx >= 0, return);
        m_buttons.at(idx)->setChecked(false);
        m_panes.value(idx)->visibilityChanged(false);
    }
}


void OutputPaneManager::shortcutTriggered()
{
    QAction *action = qobject_cast<QAction*>(sender());
    TOTEM_ASSERT(action, return);
    int idx = m_actions.indexOf(action);
    TOTEM_ASSERT(idx != -1, return);
    Core::IOutputPane *outputPane = m_panes.at(idx);

    int current = currentIndex();
    if (OutputPanePlaceHolder::isCurrentVisible() && current == idx)
    {
        if (!outputPane->hasFocus() && outputPane->canFocus())
        {
            outputPane->setFocus();
        }
        else
        {
            slotHide();
        }
    }
    else
    {

        buttonTriggered(idx);
    }
}

void OutputPaneManager::slotMinMax()
{
    OutputPanePlaceHolder *ph = OutputPanePlaceHolder::getCurrent();
    TOTEM_ASSERT(ph, return);

    if (!ph->isVisible())
        return;
    m_maximised = !m_maximised;
    ph->maximizeOrMinimize(m_maximised);
    m_minMaxAction->setIcon(m_maximised ? m_minimizeIcon : m_maximizeIcon);
    m_minMaxAction->setText(m_maximised ? tr("Minimize Output Pane")
                                            : tr("Maximize Output Pane"));

}

void OutputPaneManager::focusInEvent(QFocusEvent *e)
{
    if (QWidget *w = m_outputWidgetPane->currentWidget())
        w->setFocus(e->reason());
}
//-------------------------------------------------------------
OutputPaneToggleButton::OutputPaneToggleButton(int number, const QString &text,
                                               QAction *action, QWidget *parent)
    : QToolButton(parent)
    , m_number(QString::number(number))
    , m_text(text)
    , m_action(action)
    , m_flashTimer(new QTimeLine(1000, this))
{
    setFocusPolicy(Qt::NoFocus);
    setCheckable(true);
    QFont fnt = QApplication::font();
    setFont(fnt);
//    setStyleSheet(buttonStyleSheet());
    if (m_action)
        connect(m_action, SIGNAL(changed()), this, SLOT(updateToolTip()));

    m_flashTimer->setDirection(QTimeLine::Forward);
    m_flashTimer->setCurveShape(QTimeLine::SineCurve);
    m_flashTimer->setFrameRange(0, 92);
    connect(m_flashTimer, SIGNAL(valueChanged(qreal)), this, SLOT(update()));
    connect(m_flashTimer, SIGNAL(finished()), this, SLOT(update()));

    m_label = new QLabel(this);
    fnt.setBold(true);
    fnt.setPixelSize(11);
    m_label->setFont(fnt);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet("background-color: #818181; color: white; border-radius: 6; padding-left: 4; padding-right: 4;");
    m_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_label->hide();
}

void OutputPaneToggleButton::updateToolTip()
{
    Q_ASSERT(m_action);
    setToolTip(m_action->toolTip());
}

QSize OutputPaneToggleButton::sizeHint() const
{
    ensurePolished();

    QSize s = fontMetrics().size(Qt::TextSingleLine, m_text);

    // Expand to account for border image set by stylesheet above
    s.rwidth() += 19 + 5 + 2;
    s.rheight() += 2 + 2;

    if (!m_label->text().isNull())
        s.rwidth() += m_label->width();

    return s.expandedTo(QApplication::globalStrut());
}

void OutputPaneToggleButton::resizeEvent(QResizeEvent *event)
{
    QToolButton::resizeEvent(event);
    if (!m_label->text().isNull()) {
        m_label->move(width() - m_label->width() - 3,  (height() - m_label->height() + 1) / 2);
        m_label->show();
    }
}

void OutputPaneToggleButton::paintEvent(QPaintEvent *event)
{
    // For drawing the style sheet stuff
    QToolButton::paintEvent(event);

    const QFontMetrics fm = fontMetrics();
    const int baseLine = (height() - fm.height() + 1) / 2 + fm.ascent();
    const int numberWidth = fm.width(m_number);

    QPainter p(this);
    if (m_flashTimer->state() == QTimeLine::Running) {
        p.setPen(Qt::transparent);
        p.fillRect(rect().adjusted(19, 1, -1, -1), QBrush(QColor(255,0,0, m_flashTimer->currentFrame())));
    }
    p.setFont(font());
    p.setPen(Qt::white);
    p.drawText((20 - numberWidth) / 2, baseLine, m_number);
    if (!isChecked())
        p.setPen(Qt::black);
    int leftPart = 22;
    int labelWidth = m_label->isVisible() ? m_label->width() + 3 : 0;
    p.drawText(leftPart, baseLine, fm.elidedText(m_text, Qt::ElideRight, width() - leftPart - 1 - labelWidth));
}

void OutputPaneToggleButton::checkStateSet()
{
    //Stop flashing when button is checked
    QToolButton::checkStateSet();
    m_flashTimer->stop();

    if (isChecked())
        m_label->setStyleSheet("background-color: #e1e1e1; color: #606060; border-radius: 6; padding-left: 4; padding-right: 4;");
    else
        m_label->setStyleSheet("background-color: #818181; color: white; border-radius: 6; padding-left: 4; padding-right: 4;");
}

void OutputPaneToggleButton::flash(int count)
{
    //Start flashing if button is not checked
    if (!isChecked()) {
        m_flashTimer->setLoopCount(count);
        if (m_flashTimer->state() != QTimeLine::Running)
            m_flashTimer->start();
        update();
    }
}

void OutputPaneToggleButton::setIconBadgeNumber(int number)
{
    if (number) {
        const QString text = QString::number(number);
        m_label->setText(text);

        QSize size = m_label->sizeHint();
        if (size.width() < size.height())
            //Ensure we increase size by an even number of pixels
            size.setWidth(size.height() + ((size.width() - size.height()) & 1));
        m_label->resize(size);

        //Do not show yet, we wait until the button has been resized
    } else {
        m_label->setText(QString());
        m_label->hide();
    }
    updateGeometry();
}

///////////////////////////////////////////////////////////////////////
//
// OutputPaneManageButton
//
///////////////////////////////////////////////////////////////////////

OutputPaneManageButton::OutputPaneManageButton()
{
    setFocusPolicy(Qt::NoFocus);
    setCheckable(true);
    setStyleSheet(QLatin1String("QToolButton { border-image: url(:/core/images/about-totem.png) 2 2 2 2;"
                                " border-width: 2px 2px 2px 2px } "
                                "QToolButton::menu-indicator { width:0; height:0 }"));
}

QSize OutputPaneManageButton::sizeHint() const
{
    ensurePolished();
    return QSize(18, 18);
}

void OutputPaneManageButton::paintEvent(QPaintEvent *event)
{
    QToolButton::paintEvent(event); // Draw style sheet.
    QPainter p(this);
    QStyle *s = style();
    QStyleOption arrowOpt;
    arrowOpt.initFrom(this);
    arrowOpt.rect = QRect(5, rect().center().y() - 3, 9, 9);
    arrowOpt.rect.translate(0, -3);
    s->drawPrimitive(QStyle::PE_IndicatorArrowUp, &arrowOpt, &p, this);
    arrowOpt.rect.translate(0, 6);
    s->drawPrimitive(QStyle::PE_IndicatorArrowDown, &arrowOpt, &p, this);
}
