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
		addRegion(processor->name(), pWidget);
}

void DesignNetFrontWidget::onProcessorRemoved(Processor* processor)
{
	ProcessorFrontWidget* pWidget = processor->processorFrontWidget();
	int iIndex = m_vecWidget.indexOf(pWidget);
	if (iIndex >= 0)
		removeRegion(iIndex);
}

}
