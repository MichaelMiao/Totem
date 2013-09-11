#include "processorconfigwidget.h"
#include "processorconfigpage.h"
#include "processorpropertywidget.h"
#include "processorconfiglistener.h"
#include "designnetbase/processor.h"
#include <QTabWidget>
#include <QVBoxLayout>
namespace DesignNet{
ProcessorConfigWidget::ProcessorConfigWidget(Processor* processor, QWidget *parent)
	: QWidget(parent),
	m_processor(processor)
{
	setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
	setWindowTitle(tr("Configuration"));
	m_tabWidget = new QTabWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_tabWidget);
	this->setLayout(layout);
}

ProcessorConfigWidget::~ProcessorConfigWidget()
{

}

void ProcessorConfigWidget::addPage( ProcessorConfigPage* page )
{
	QString strTitle;
	if (page->pageType() == ProcessorConfigPage::PageType_Permanent)
	{
		strTitle = tr("Setting");	
	}
	else
	{
		strTitle = tr("Property");
	}
	connect(page, SIGNAL(configFinished()), this, SLOT(onSettingsChanged()));
	m_tabWidget->addTab(page, strTitle);
}

ProcessorConfigWidget* ProcessorConfigWidget::create( Processor* processor, QWidget *parent /*= 0*/ )
{
	return 0;
}

void ProcessorConfigWidget::addPropertyPage( Processor* processor )
{
	if (processor->getProperties().count() == 0)
	{
		return ;
	}
	ProcessorConfigPage* page = new ProcessorConfigPage(ProcessorConfigPage::PageType_RealTime, this);
	ProcessorPropertyWidget *widget = new ProcessorPropertyWidget(processor, this);
	widget->initWidgets();
	page->addRegion(QString(), widget);
	addPage(page);
}

void ProcessorConfigWidget::addListener( ProcessorConfigListener *listener )
{
	m_listeners.append(listener);
}

void ProcessorConfigWidget::onSettingsChanged()
{
	ProcessorConfigPage* page = qobject_cast<ProcessorConfigPage*>(sender());
	if (page)
	{
		foreach(ProcessorConfigListener* listener, m_listeners)
		{
			listener->onSettingsChanged();
		}
	}
}

void ProcessorConfigWidget::init()
{

}

}
