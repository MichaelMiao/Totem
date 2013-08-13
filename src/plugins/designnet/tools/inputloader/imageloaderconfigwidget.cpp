#include "imageloaderconfigwidget.h"
#include "CustomUI/pathchooser.h"
#include "widgets/processorconfigpage.h"
#include "graphicsnormalimageloader.h"
using namespace CustomUI;
using namespace DesignNet;
using namespace InputLoader;

ImageLoaderConfigListener::ImageLoaderConfigListener( ImageLoaderConfigWidget *widget, InputLoader::GraphicsNormalImageLoader *processor, QObject *parent )
	: ProcessorConfigListener(parent)
{
	m_processor = processor;
	m_widget	= widget;
}

void ImageLoaderConfigListener::onSettingsChanged()
{
	m_processor->setPath(m_widget->getFilePath());
}

void ImageLoaderConfigListener::onPropertyChanged( DesignNet::Property *prop )
{

}

ImageLoaderConfigWidget::ImageLoaderConfigWidget( Processor* processor, QWidget *parent )
	: ProcessorConfigWidget(processor, parent)
{

}

ImageLoaderConfigWidget::~ImageLoaderConfigWidget()
{

}

ProcessorConfigWidget* ImageLoaderConfigWidget::create( DesignNet::Processor * processor, QWidget *parent /*= 0*/ )
{
	ImageLoaderConfigWidget *widget = new ImageLoaderConfigWidget(processor, parent);
	ProcessorConfigPage *page = new ProcessorConfigPage(ProcessorConfigPage::PageType_Permanent, parent);
	page->setApplyButtonEnabled(false);

	m_pathChooser = new PathChooser(this);
	m_pathChooser->setExpectedKind(PathChooser::File);
	m_pathChooser->setPromptDialogFilter("Images (*.bmp *.xpm *.jpg)");
	
	page->setApplyButtonEnabled(false);
	connect(m_pathChooser, SIGNAL(validChanged(bool)), page, SLOT(setApplyButtonEnabled(bool)));
	page->addRegion(tr("Image File Path"), m_pathChooser);
	widget->addPage(page);	

	GraphicsNormalImageLoader *loader = (GraphicsNormalImageLoader *)processor;
	m_pathChooser->setPath(loader->path());
	m_listener = new ImageLoaderConfigListener(this, loader, this);
	widget->addListener(m_listener);
	return widget;
}

QString ImageLoaderConfigWidget::getFilePath()
{
	return m_pathChooser->path();
}
