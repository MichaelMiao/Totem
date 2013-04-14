#include "rightpane.h"
#include "imode.h"
#include "modemanager.h"

#include <QVBoxLayout>
#include <QSplitter>
#include <QSettings>
#include <QResizeEvent>

namespace Core{

RightPanePlaceHolder *RightPanePlaceHolder::m_current = 0;

RightPanePlaceHolder *RightPanePlaceHolder::current()
{
    return m_current;
}

RightPanePlaceHolder::RightPanePlaceHolder(IMode *mode, QWidget *parent)
    : QWidget(parent), m_mode(mode)
{
    setLayout(new QVBoxLayout);
    layout()->setMargin(0);
    connect(Core::ModeManager::instance(), SIGNAL(currentModeChanged(Core::IMode*)),
            this, SLOT(currentModeChanged(Core::IMode*)));

}

RightPanePlaceHolder::~RightPanePlaceHolder()
{
    if (m_current == this)
    {
        RightPaneWidget::instance()->setParent(0);
        RightPaneWidget::instance()->hide();
    }
}

void RightPanePlaceHolder::currentModeChanged(IMode *mode)
{
    if (m_current == this)
    {
        m_current = 0;
        RightPaneWidget::instance()->setParent(0);
        RightPaneWidget::instance()->hide();
    }
    if (m_mode == mode)
    {
        m_current = this;

        int width = RightPaneWidget::instance()->storedWidth();

        layout()->addWidget(RightPaneWidget::instance());
        RightPaneWidget::instance()->show();

        applyStoredSize(width);
        setVisible(RightPaneWidget::instance()->isShown());
    }
}

void RightPanePlaceHolder::applyStoredSize(int width)
{
    if (width)
    {
        QSplitter *splitter = qobject_cast<QSplitter *>(parentWidget());
        if (splitter)
        {
            // A splitter we need to resize the splitter sizes
            QList<int> sizes = splitter->sizes();
            int index = splitter->indexOf(this);
            int diff = width - sizes.at(index);
            int adjust = sizes.count() > 1 ? (diff / (sizes.count() - 1)) : 0;
            for (int i = 0; i < sizes.count(); ++i)
            {
                if (i != index)
                    sizes[i] -= adjust;
            }
            sizes[index]= width;
            splitter->setSizes(sizes);
        }
        else
        {
            QSize s = size();
            s.setWidth(width);
            resize(s);
        }
    }
}
//-----------------------------------------------------------
RightPaneWidget *RightPaneWidget::m_instance = 0;

RightPaneWidget::RightPaneWidget()
    : m_shown(true), m_width(0)
{
    m_instance = this;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    setLayout(layout);
}

RightPaneWidget::~RightPaneWidget()
{
    clearWidget();
    m_instance = 0;
}

void RightPaneWidget::saveSettings(QSettings *settings)
{
    settings->setValue(QLatin1String("RightPane/Visible"), isShown());
    settings->setValue(QLatin1String("RightPane/Width"), m_width);
}

void RightPaneWidget::readSettings(QSettings *settings)
{
    if (settings->contains(QLatin1String("RightPane/Visible")))
    {
        setShown(settings->value(QLatin1String("RightPane/Visible")).toBool());
    }
    else
    {
        setShown(false);
    }

    if (settings->contains(QLatin1String("RightPane/Width")))
    {
        m_width = settings->value(QLatin1String("RightPane/Width")).toInt();
        if (!m_width)
            m_width = 500;
    }
    else
    {
        m_width = 500; //pixel
    }
    // Apply
    if (RightPanePlaceHolder::m_current)
    {
        RightPanePlaceHolder::m_current->applyStoredSize(m_width);
    }
}

bool RightPaneWidget::isShown()
{
    return m_shown;
}

void RightPaneWidget::setShown(bool b)
{
    if (RightPanePlaceHolder::m_current)
        RightPanePlaceHolder::m_current->setVisible(b);
    m_shown = b;
}

RightPaneWidget *RightPaneWidget::instance()
{
    return m_instance;
}

void RightPaneWidget::setWidget(QWidget *widget)
{
    clearWidget();
    m_widget = widget;
    if (m_widget)
    {
        m_widget->setParent(this);
        layout()->addWidget(m_widget);
        setFocusProxy(m_widget);
        m_widget->show();
    }
}

int RightPaneWidget::storedWidth()
{
    return m_width;
}

void RightPaneWidget::resizeEvent(QResizeEvent *re)
{
    if (m_width && re->size().width())
        m_width = re->size().width();
    QWidget::resizeEvent(re);
}

void RightPaneWidget::clearWidget()
{
    if (m_widget)
    {
        m_widget->hide();
        m_widget->setParent(0);
        m_widget = 0;
    }
}

}
