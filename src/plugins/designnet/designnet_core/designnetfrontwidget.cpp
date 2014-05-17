#include "designnetfrontwidget.h"
#include <QResizeEvent>
#include "designnetbase/designnetspace.h"
#include "designnetdocument.h"
#include "designnetbase/processor.h"
#include "designneteditor.h"


namespace DesignNet
{


DesignNetFrontWidget::DesignNetFrontWidget(DesignNetEditor* pEditor)
	: m_pEditor(pEditor)
{
	DesignNetDocument* pDocument = (DesignNetDocument*)m_pEditor->document();
	connect(pDocument, SIGNAL(deserialized(Utils::XmlDeserializer &)), 
		this, SLOT(onDesignNetFinished()));
	m_pSpace = pDocument->designNetSpace();

	connect(m_pSpace, SIGNAL(processorAdded(Processor*)), 
		this, SLOT(onProcessorAdded(Processor*)));
	connect(m_pSpace, SIGNAL(processorRemoved(Processor*)), 
		this, SLOT(onProcessorRemoved(Processor*)));

	setWidget(&m_widget);
	setWidgetResizable(true);
}

DesignNetFrontWidget::~DesignNetFrontWidget()
{

}

void DesignNetFrontWidget::onDesignNetFinished()
{
// 	QList<Processor*> ls = m_pSpace->processors();
// 	for (int i = 0; i < ls.size(); i++)
// 	{
// 		if (ls.at(i)->processorFrontWidget())
// 			addRegion(ls.at(i)->name(), ls.at(i)->processorFrontWidget());
// 	}
}

void DesignNetFrontWidget::onProcessorAdded(Processor* processor)
{
	ProcessorFrontWidget* pWidget = processor->processorFrontWidget();
	if (pWidget && !m_vecWidget.contains(pWidget))
	{
		m_widget.addRegion(processor->name(), pWidget);
		m_vecWidget.push_back(pWidget);
	}
}

void DesignNetFrontWidget::onProcessorRemoved(Processor* processor)
{
	ProcessorFrontWidget* pWidget = processor->processorFrontWidget();
	int iIndex = m_vecWidget.indexOf(pWidget);
	if (iIndex >= 0)
		m_widget.removeRegion(iIndex);
}

void DesignNetFrontWidget::resizeEvent(QResizeEvent* e)
{
	QSize sz = m_widget.sizeHint();
	m_widget.setGeometry(0, 0, e->size().width(), sz.height());
}

}
