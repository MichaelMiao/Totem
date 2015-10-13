#include "stdafx.h"
#include "rebar.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QToolBar>
#include <QVBoxLayout>
#include "rebarmanager.h"

#define STYLE_TABWIDGET "QTabWidget::pane{ \
border:none; \
}"

RebarCtrl::RebarCtrl(QWidget* pParent) : QWidget(pParent)
{
	QVBoxLayout* pBox = new QVBoxLayout();
	m_pTab = new QTabWidget(this);
	m_pTab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
	m_pTab->setStyleSheet(STYLE_TABWIDGET);
	m_pToolbarLayout	= new CustomUI::FlowLayout();
	setLayout(pBox);
	pBox->addWidget(m_pTab);
	pBox->addLayout(m_pToolbarLayout);
	connect(RebarManager::instance(), SIGNAL(rebarObjectAdded(const Core::Id &, Core::Command*)),
		this, SLOT(onRebarObjectAdded(const Core::Id &, Core::Command*)));

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	setFixedHeight(100);
	setAttribute(Qt::WA_NoSystemBackground);
}

RebarCtrl::~RebarCtrl()
{
}

void RebarCtrl::onRebarObjectAdded(const Core::Id & id, Core::Command* pCmd)
{
	int iCount = m_pTab->count();
	bool bFind = false;
	int iIndex = -1;
	for (int i = 0; i < iCount; ++i)
	{
		if (m_pTab->tabText(i) == id.toString())
		{
			iIndex = i;
			bFind = true;
			break;
		}
	}
	
	QToolBar* pBar = NULL;
	if (!bFind)
	{
		pBar = new QToolBar(m_pTab);
		iIndex = m_pTab->addTab(pBar, id.toString());
	}

	if (pBar)
		pBar->addAction(pCmd->action());
}

QSize RebarCtrl::sizeHint() const
{
	return QSize(100, 100);
}

void RebarCtrl::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	p.fillRect(rect(), QColor(Qt::blue));
}
