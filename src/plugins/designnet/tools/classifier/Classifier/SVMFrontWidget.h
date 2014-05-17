#pragma once

#include <QLabel>
#include <QTableWidget>
#include <QWebView>
#include <QWidget>
#include "../../../../designnet/designnet_core/widgets/processorfrontwidget.h"
#include "opencv2/core/core.hpp"



class SVMClassifer;
class SVMFrontWidget : public DesignNet::ProcessorFrontWidget
{
	Q_OBJECT

public:

	SVMFrontWidget(SVMClassifer *processor);
	~SVMFrontWidget();

public slots:
	
	void onClassifyFinished(cv::Mat);
	void onCellDoubleClicked(int row, int column);

private:

	QWebView*			m_pWebView;
	QTableWidget*		m_pTable;
	QVector<QLabel*>	m_vecLabel;
	QMap<int, QString>	m_mapUrls;
	SVMClassifer*		m_processor;
};
