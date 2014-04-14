#include "stdafx.h"
#include "SVMFrontWidget.h"
#include <QHBoxLayout>
#include "SVMClassifier.h"


SVMFrontWidget::SVMFrontWidget(SVMClassifer *processor)
	: DesignNet::ProcessorFrontWidget(processor), m_processor(processor)
{
	m_pWebView = new QWebView(this);
	QHBoxLayout* pLayout = new QHBoxLayout;
	pLayout->addWidget(m_pWebView);
	setLayout(pLayout);
}

SVMFrontWidget::~SVMFrontWidget()
{

}
