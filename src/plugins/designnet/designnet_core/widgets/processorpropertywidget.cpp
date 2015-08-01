#include "processorpropertywidget.h"
#include "processor.h"
#include "property/property.h"
#include "property/propertymanager.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"

#include <QVBoxLayout>

namespace DesignNet{

ProcessorPropertyWidget::ProcessorPropertyWidget(Processor *processor, QWidget *parent) :
    QWidget(parent),
    m_processor(processor)
{

}

ProcessorPropertyWidget::~ProcessorPropertyWidget()
{
}

void ProcessorPropertyWidget::initWidgets()
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    this->setLayout(vLayout);
    QList<Property*> properties = m_processor->getProperties();
    foreach(Property* property, properties)
    {
        IPropertyWidget *widget = PropertyManager::instance()->createWidget(property, this);
        if(!widget)
        {
            QString error = tr("The property %1 has not been supported!").arg(property->name());
            LOGOUT(error);
            continue;
        }
        vLayout->addWidget((QWidget*)widget);
    }
}
}
