#include "uicollapsiblewidget.h"
#include "collapsiblebutton.h"

#include <QVBoxLayout>
#include <QTableWidget>
#include <QSpacerItem>

namespace CustomUI{

class uiCollapsibleWidgetPrivate
{
public:
    uiCollapsibleWidgetPrivate(uiCollapsibleWidget *parent);

    uiCollapsibleWidget*                q;
    QBoxLayout*                         m_layout;
    QSpacerItem*                        m_spacer;
    Qt::Alignment                       m_alignment;

};

uiCollapsibleWidgetPrivate::uiCollapsibleWidgetPrivate(uiCollapsibleWidget *parent)
    : q(parent),
      m_layout(0),
      m_spacer(new QSpacerItem(100, 100, QSizePolicy::Expanding, QSizePolicy::Expanding))
{

}

uiCollapsibleWidget::uiCollapsibleWidget(Qt::Alignment alignment, QWidget *parent) :
    QWidget(parent),
    d(new uiCollapsibleWidgetPrivate(this))
{
    if(alignment == Qt::AlignTop || alignment == Qt::AlignBottom)
    {
        d->m_layout = new QVBoxLayout;
    }
    else
    {
        d->m_layout = new QHBoxLayout;
    }
    d->m_alignment = alignment;
    d->m_layout->setSpacing(0);
    d->m_layout->setMargin(0);
	this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    d->m_layout->addSpacerItem(d->m_spacer);
    this->setLayout(d->m_layout);
}

void uiCollapsibleWidget::addRegion(const QString &text, QWidget *widget)
{
    insertRegion(text, widget, 0);
}

void uiCollapsibleWidget::addRegion(const QString &text, QLayout *pLayout)
{
    insertRegion(text, pLayout, 0);
}

void uiCollapsibleWidget::insertRegion(const QString &text, QWidget *widget, int index)
{
	if (text.isEmpty())
	{
		d->m_layout->insertWidget(index, widget);
	}
	else
	{
		CollapsibleButton *pButton = new CollapsibleButton(text, this);
		QVBoxLayout *vLayout = new QVBoxLayout;
		if(widget)
			vLayout->addWidget(widget);

		pButton->setLayout(vLayout);
		d->m_layout->insertWidget(index, pButton);
	}
}

void uiCollapsibleWidget::insertRegion(const QString &text, QLayout *pLayout, int index)
{
    CollapsibleButton *pButton = new CollapsibleButton(text, this);
    pButton->setLayout(pLayout);
    d->m_layout->insertWidget(index, pButton);
}

void uiCollapsibleWidget::removeRegion(const int &index)
{
    int i = 0;
    foreach(QObject *obj, d->m_layout->children())
    {
        CollapsibleButton *pButton = qobject_cast<CollapsibleButton*>(obj);
        if(pButton)
        {
            if(i == index)
            {
                d->m_layout->removeWidget(pButton);
                return;
            }
            i++;
        }
    }
}

}
