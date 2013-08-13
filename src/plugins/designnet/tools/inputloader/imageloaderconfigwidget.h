#ifndef IMAGELOADERCONFIGWIDGET_H
#define IMAGELOADERCONFIGWIDGET_H

#include <QObject>

#include "widgets/processorconfigwidget.h"

#include "designnet/designnet_core/processorconfiglistener.h"
namespace InputLoader{
class GraphicsNormalImageLoader;
}

namespace CustomUI{
	class PathChooser;
}
class ImageLoaderConfigWidget;
class ImageLoaderConfigListener: public DesignNet::ProcessorConfigListener
{
public:
	ImageLoaderConfigListener(ImageLoaderConfigWidget *widget, InputLoader::GraphicsNormalImageLoader *processor, QObject *parent);
	virtual void onSettingsChanged();
	virtual void onPropertyChanged(DesignNet::Property *prop);
	InputLoader::GraphicsNormalImageLoader*	m_processor;
	ImageLoaderConfigWidget *				m_widget;

};

class ImageLoaderConfigWidget : public DesignNet::ProcessorConfigWidget
{
	Q_OBJECT

public:
	ImageLoaderConfigWidget(DesignNet::Processor* processor, QWidget *parent);
	~ImageLoaderConfigWidget();
	virtual ProcessorConfigWidget* create(DesignNet::Processor * processor, QWidget *parent = 0);
	QString getFilePath();
private:
	DesignNet::ProcessorConfigListener *m_listener;
	CustomUI::PathChooser *				m_pathChooser;
};

#endif // IMAGELOADERCONFIGWIDGET_H
