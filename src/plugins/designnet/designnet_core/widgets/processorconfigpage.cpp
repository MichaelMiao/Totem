#include "processorconfigpage.h"
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QPushButton>
using namespace CustomUI;
namespace DesignNet{

ProcessorConfigPage::ProcessorConfigPage( PageType pt, QWidget *parent )
	: uiCollapsibleWidget(Qt::AlignTop, parent),
	m_pageType(pt)
{
	m_buttonBox = 0;
	if (pt == PageType_Permanent)
	{
		m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Apply| QDialogButtonBox::Cancel, Qt::Horizontal);
		addRegion(QString(), m_buttonBox);
		connect(m_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SIGNAL(configFinished()));
	}
}

ProcessorConfigPage::~ProcessorConfigPage()
{

}

ProcessorConfigPage::PageType ProcessorConfigPage::pageType()
{
	return m_pageType;
}

void ProcessorConfigPage::setApplyButtonEnabled( bool bEnabled )
{
	if (m_buttonBox)
	{
		m_buttonBox->button(QDialogButtonBox::Apply)->setEnabled(bEnabled);
	}
}

}
