#include "featuresaverconfig.h"
#include "widgets/processorconfigpage.h"
#include "CustomUI/pathchooser.h"
#include "CustomUI/filenamevalidatinglineedit.h"
#include "featuresaver.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
using namespace DesignNet;
using namespace CustomUI;


FeatureSaverConfigListener::FeatureSaverConfigListener( FeatureSaverConfig *widget, Processor *processor)
	: ProcessorConfigListener(widget), m_widget(widget)
{
	m_processor = (FeatureSaver*)processor;
}

void FeatureSaverConfigListener::onSettingsChanged()
{

}

void FeatureSaverConfigListener::onPropertyChanged( DesignNet::Property *prop )
{

}

//////////////////////////////////////////////////////////////////////////

FeatureSaverConfig::FeatureSaverConfig(DesignNet::Processor* processor, QWidget *parent)
	: ProcessorConfigWidget(processor, parent)
{
	ui.setupUi(this);
	m_pathChooser	= 0;
}

FeatureSaverConfig::~FeatureSaverConfig()
{

}

ProcessorConfigWidget* FeatureSaverConfig::create(DesignNet::Processor * processor, QWidget *parent)
{
	FeatureSaverConfig * widget = new FeatureSaverConfig(processor, parent);
	return widget;
}

QString FeatureSaverConfig::getPath()
{
	return "";
}

void FeatureSaverConfig::init()
{
	m_pathChooser = new CustomUI::PathChooser(this);
	m_nameValidatingLineEdit = new FileNameValidatingLineEdit(this);
	m_listener = new FeatureSaverConfigListener(this, m_processor);
	QGridLayout* pLayout = new QGridLayout(this);
	pLayout->addWidget(new QLabel("File Name", this));
	pLayout->addWidget(m_nameValidatingLineEdit, 0, 1);
	pLayout->addWidget(new QLabel("Path", this));
	pLayout->addWidget(m_pathChooser, 1, 1);
	ProcessorConfigPage *page = new ProcessorConfigPage(ProcessorConfigPage::PageType_Permanent, this);
	page->setApplyButtonEnabled(false);
	connect(m_pathChooser, SIGNAL(validChanged(bool)), page, SLOT(setApplyButtonEnabled(bool)));
	page->addRegion(tr("FeatureSaver"), pLayout);
	this->addPage(page);
	addListener(m_listener);
}
