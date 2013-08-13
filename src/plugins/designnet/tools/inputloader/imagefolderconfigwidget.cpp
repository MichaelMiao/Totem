#include "imagefolderconfigwidget.h"
#include "widgets/processorconfigpage.h"
#include "CustomUI/pathchooser.h"
#include "designnetbase/processor.h"
#include "ImageFolderLoader.h"
using namespace DesignNet;
using namespace CustomUI;
using namespace InputLoader;
ImageFolderConfigListener::ImageFolderConfigListener( ImageFolderConfigWidget *widget, InputLoader::ImageFolderLoader *processor, QObject *parent )
	: ProcessorConfigListener(parent)
{
	m_processor = processor;
	m_widget	= widget;
}

void ImageFolderConfigListener::onSettingsChanged()
{
	QString path = m_widget->getPath();
	m_processor->setPath(path);
}

void ImageFolderConfigListener::onPropertyChanged( Property *prop )
{

}

ImageFolderConfigWidget::ImageFolderConfigWidget(Processor* processor, QWidget *parent)
	: ProcessorConfigWidget(processor, parent)
{
	m_listener		= 0;
	m_pathChooser	= 0;
}

ImageFolderConfigWidget::~ImageFolderConfigWidget()
{

}

ProcessorConfigWidget* ImageFolderConfigWidget::create( DesignNet::Processor * processor, QWidget *parent /*= 0*/ )
{
	ImageFolderConfigWidget * widget = new ImageFolderConfigWidget(processor, parent);
	ProcessorConfigPage *page = new ProcessorConfigPage(ProcessorConfigPage::PageType_Permanent, parent);
	
	m_pathChooser = new PathChooser(this);
	page->setApplyButtonEnabled(false);
	
	connect(m_pathChooser, SIGNAL(validChanged(bool)), page, SLOT(setApplyButtonEnabled(bool)));
	page->addRegion(tr("Image Folder Path"), m_pathChooser);
	widget->addPage(page);	
	ImageFolderLoader *loader = (ImageFolderLoader *)processor;
	m_pathChooser->setPath(loader->path());
	m_listener = new ImageFolderConfigListener(this, loader, this);
	widget->addListener(m_listener);
	return widget;
}

QString ImageFolderConfigWidget::getPath()
{
	return m_pathChooser->path();
}
