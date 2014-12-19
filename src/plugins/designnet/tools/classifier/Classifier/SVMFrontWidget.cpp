#include "stdafx.h"
#include "SVMFrontWidget.h"
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include "extensionsystem/pluginmanager.h"
#include "SVMClassifier.h"
#include "designnet/designnet_core/data/customdata.h"


static const char chId[] = "svm_result";
using namespace DesignNet;
SVMFrontWidget::SVMFrontWidget(SVMClassifer *processor)
	: DesignNet::ProcessorFrontWidget(processor), m_processor(processor)
{
	m_pWebView	= new QWebView(this);
	m_pWebView->setBackgroundRole(QPalette::Dark);
	m_pWebView->setAutoFillBackground(true);
	m_pTable	= new QTableWidget(this);
	QHBoxLayout* pLayout = new QHBoxLayout;
	pLayout->addWidget(m_pTable);
	pLayout->addWidget(m_pWebView);
	setLayout(pLayout);
	setMinimumSize(200, 200);
	connect(processor, SIGNAL(classifyFinished(cv::Mat)), this, SLOT(onClassifyFinished(cv::Mat)));
	m_pTable->setColumnCount(2);
	m_pTable->setSelectionMode(QAbstractItemView::SingleSelection);
	m_pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	connect(m_pTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onCellDoubleClicked(int, int)));

	QFile file("I:/data/info.txt");
	if (file.open(QFile::ReadOnly))
	{
		int iIndex;
		QString strUrl;
		QTextStream s(&file);
		s >> iIndex >> strUrl;
		m_mapUrls[iIndex] = strUrl;
		file.close();
	}

}

SVMFrontWidget::~SVMFrontWidget()
{

}

void SVMFrontWidget::onClassifyFinished(cv::Mat res)
{
	m_pTable->clearContents();
	for (int i = 0; i < res.cols; i++)
	{
		m_pTable->insertRow(i);
		QTableWidgetItem* pItem = new QTableWidgetItem;
		pItem->setText(tr("%1").arg(res.at<int>(0, i)));
		m_pTable->setItem(i, 0, pItem);
		pItem->setData(Qt::UserRole, qVariantFromValue(i));
	}
	CustomData* pData = new CustomData;
	pData->setId(chId);
	pData->setData((void*)&res, false);
	ExtensionSystem::PluginManager::instance()->addObject(pData);
}

void SVMFrontWidget::onCellDoubleClicked(int row, int column)
{
	if (m_mapUrls.count() > row)
		m_pWebView->load(QUrl(tr("%1").arg(m_mapUrls[column])));
}
