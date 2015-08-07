#include "processorconfigwidget.h"
#include <QShowEvent>
#include <QTabWidget>
#include <QVBoxLayout>
#include "../designnetbase/processor.h"
#include "../processorconfiglistener.h"
#include "processorconfigpage.h"
#include "processorpropertywidget.h"


namespace DesignNet{
ProcessorConfigWidget::ProcessorConfigWidget(Processor* processor, QWidget *parent, bool bPropertyPage)
	: QWidget(parent),
	m_processor(processor)
{
	setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
	setWindowTitle(tr("Configuration"));
	setFocusPolicy(Qt::StrongFocus);
	m_tabWidget = new QTabWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_tabWidget);
	this->setLayout(layout);
	if (bPropertyPage)
		addPropertyPage();
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

void ProcessorConfigWidget::addPropertyPage()
{
	if (m_processor->getProperties().count() == 0)
		return ;

	ProcessorConfigPage* page = new ProcessorConfigPage(ProcessorConfigPage::PageType_RealTime, this);
	ProcessorPropertyWidget *widget = new ProcessorPropertyWidget(m_processor, this);
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

void ProcessorConfigWidget::showEvent(QShowEvent *event)
{
	if (this->isVisible())
		setFocus();
}

}
