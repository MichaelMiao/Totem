#include "processorpropertywidget.h"
#include "property.h"
#include "processor.h"
#include "propertymanager.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"

#include <QVBoxLayout>

using namespace CustomUI;
namespace DesignNet{

ProcessorPropertyWidget::ProcessorPropertyWidget(Processor *processor, QWidget *parent) :
    CollapsibleButton(parent),
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
	QString title = tr("ID: %1 Type: %2").arg(m_processor->id()).arg(m_processor->name());
	setText(title);
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
