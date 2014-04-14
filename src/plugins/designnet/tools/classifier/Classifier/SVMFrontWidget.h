#pragma once

#include <QWidget>
#include <QLabel>
#include "designnet/designnet_core/widgets/processorfrontwidget.h"
#include <QWebView>

class SVMClassifer;
class SVMFrontWidget : public DesignNet::ProcessorFrontWidget
{
	Q_OBJECT

public:

	SVMFrontWidget(SVMClassifer *processor);
	~SVMFrontWidget();


private:

	QWebView*		m_pWebView;

	SVMClassifer*	m_processor;
};