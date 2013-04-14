#include "infobar.h"
#include "coreconstants.h"

#include <QWidget>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
namespace Core{

InfoBarEntry::InfoBarEntry(const QString &id, const QString &infoText)
    : m_id(id),
      m_infoText(infoText),
      m_object(0),
      m_buttonPressMember(0),
      m_cancelObject(0),
      m_cancelButtonPressMember(0)
{

}

void InfoBarEntry::setCustomButtonInfo(const QString &_buttonText, QObject *object, const char *member)
{
    m_buttonText = _buttonText;
    m_object = object;
    m_buttonPressMember = member;
}

void InfoBarEntry::setCancelButtonInfo(QObject *object, const char *member)
{
    m_cancelObject = object;
    m_cancelButtonPressMember = member;
}

void InfoBarEntry::setCancelButtonInfo(const QString &cancelButtonText, QObject *object, const char *member)
{
    m_cancelButtonText = cancelButtonText;
    m_cancelObject = object;
    m_cancelButtonPressMember = member;
}

void InfoBar::addInfo(const InfoBarEntry &info)
{
    m_infoBarEntries << info;
    emit changed();
}

void InfoBar::removeInfo(const QString &id)
{
    QMutableListIterator<InfoBarEntry> itr(m_infoBarEntries);
    while (itr.hasNext())
    {
        if(itr.next().m_id == id)
        {
            itr.remove();
            emit changed();
            return;
        }
    }
}

void InfoBar::clear()
{
    if(!m_infoBarEntries.isEmpty())
    {
        m_infoBarEntries.clear();
        emit changed();
    }
}

InfoBarDisplay::InfoBarDisplay(QObject *parent)
    : QObject(parent),
      m_infoBar(0),
      m_boxLayout(0),
      m_boxIndex(0)
{
}

void InfoBarDisplay::setTarget(QBoxLayout *layout,
                               int index)
{
    m_boxIndex = index;
    m_boxLayout = layout;
}

void InfoBarDisplay::setInfoBar(InfoBar *infoBar)
{
    if(m_infoBar == infoBar)
        return;

    if(m_infoBar)
        m_infoBar->disconnect(this);
    m_infoBar = infoBar;
    if(m_infoBar)
    {
        connect(infoBar, SIGNAL(changed()),SLOT(update()));
        connect(infoBar, SIGNAL(destroyed()), SLOT(infoBarDestroyed()));
    }
    update();
}

void InfoBarDisplay::cancelButtonClicked()
{
    m_infoBar->removeInfo(sender()->property("infoId").toString());
}

void InfoBarDisplay::update()
{
    foreach (QWidget *widget, m_infoWidgets)
    {
        widget->disconnect(this);
        delete widget;
    }
    m_infoWidgets.clear();

    if (!m_infoBar)
        return;

    foreach (const InfoBarEntry &info, m_infoBar->m_infoBarEntries)
    {
        QFrame *infoWidget = new QFrame;

        QPalette pal = infoWidget->palette();
        pal.setColor(QPalette::Window, QColor(255, 255, 225));
        pal.setColor(QPalette::WindowText, Qt::black);

        infoWidget->setPalette(pal);
        infoWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);
        infoWidget->setLineWidth(1);
        infoWidget->setAutoFillBackground(true);

        QHBoxLayout *hbox = new QHBoxLayout(infoWidget);
        hbox->setMargin(2);

        QLabel *infoWidgetLabel = new QLabel(info.m_infoText);
        infoWidgetLabel->setWordWrap(true);
        hbox->addWidget(infoWidgetLabel);

        if (!info.m_buttonText.isEmpty())
        {
            QToolButton *infoWidgetButton = new QToolButton;
            infoWidgetButton->setText(info.m_buttonText);
            connect(infoWidgetButton, SIGNAL(clicked()), info.m_object, info.m_buttonPressMember);

            hbox->addWidget(infoWidgetButton);
        }

        QToolButton *infoWidgetCloseButton = new QToolButton;
        infoWidgetCloseButton->setProperty("infoId", info.m_id);

        if (info.m_cancelObject)
            connect(infoWidgetCloseButton, SIGNAL(clicked()),
                    info.m_cancelObject, info.m_cancelButtonPressMember);
        connect(infoWidgetCloseButton, SIGNAL(clicked()), SLOT(cancelButtonClicked()));

        if (info.m_cancelButtonText.isEmpty())
        {
            infoWidgetCloseButton->setAutoRaise(true);
            infoWidgetCloseButton->setIcon(QIcon(QLatin1String(Core::Constants::ICON_CLOSE_DOCUMENT)));
            infoWidgetCloseButton->setToolTip(tr("Close"));
        }
        else
        {
            infoWidgetCloseButton->setText(info.m_cancelButtonText);
        }

        hbox->addWidget(infoWidgetCloseButton);

        connect(infoWidget, SIGNAL(destroyed()), SLOT(widgetDestroyed()));
        m_boxLayout->insertWidget(m_boxIndex, infoWidget);
        m_infoWidgets << infoWidget;
    }
}

void InfoBarDisplay::infoBarDestroyed()
{
    m_infoBar = 0;
}

void InfoBarDisplay::widgetDestroyed()
{
    m_infoWidgets.clear();
}


}
