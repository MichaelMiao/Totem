#include "propertylistwidget.h"
#include "processor.h"
#include "processorpropertywidget.h"
#include <QHash>
#include <QVBoxLayout>
namespace DesignNet{
class PropertyListWidgetPrivate{
public:
    PropertyListWidgetPrivate(PropertyListWidget* widget);
    QWidget*        m_widget;
    QVBoxLayout*    m_mainLayout;
    QHash<Processor*, ProcessorPropertyWidget*> m_processorPropertyWidgets;
};

PropertyListWidgetPrivate::PropertyListWidgetPrivate(PropertyListWidget *widget)
{
    m_widget = new QWidget(widget);
    m_mainLayout = new QVBoxLayout(m_widget);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(1, 1, 1, 1);
    m_widget->setLayout(m_mainLayout);
    m_mainLayout->setAlignment(Qt::AlignTop);
}


PropertyListWidget::PropertyListWidget(QWidget *parent) :
    QScrollArea(parent),
    d(new PropertyListWidgetPrivate(this))
{
    setWidget(d->m_widget);
    setWidgetResizable(true);
    d->m_widget->show();
}

QSize PropertyListWidget::sizeHint() const
{
    return QSize(200, 0);
}

void PropertyListWidget::processorAdded(Processor *processor)
{
    ProcessorPropertyWidget *widget = new ProcessorPropertyWidget(processor, d->m_widget);
    widget->initWidgets();
    d->m_processorPropertyWidgets.insert(processor, widget);
    widget->setVisible(false);
    d->m_mainLayout->addWidget(widget);
}

void PropertyListWidget::processorSelected(QList<Processor *> selectedProcessors)
{
    QHash<Processor*, ProcessorPropertyWidget*>::iterator itr = d->m_processorPropertyWidgets.begin();
    for(; itr != d->m_processorPropertyWidgets.end();++itr)
    {
        ProcessorPropertyWidget * widget = itr.value();
        QList<Processor *>::const_iterator itrSelected = qFind(selectedProcessors, itr.key());
        if(itrSelected != selectedProcessors.constEnd())
        {
            widget->setVisible(true);
            widget->setCollapsed(false);
        }
        else
        {
            widget->setVisible(false);
        }
    }

}


}
