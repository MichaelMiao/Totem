#ifndef IMAGEFOLDERCONFIGWIDGET_H
#define IMAGEFOLDERCONFIGWIDGET_H

#include "stdafx.h"
#include "designnet/designnet_core/widgets/processorconfigwidget.h"
#include "designnet/designnet_core/processorconfiglistener.h"

#include <QWidget>


namespace DesignNet{
class Processor;
class Property;

}

namespace CustomUI{
class PathChooser;
}
namespace InputLoader{
class ImageFolderLoader;
}
class ImageFolderConfigWidget;
class ImageFolderConfigListener: public DesignNet::ProcessorConfigListener
{
public:
	ImageFolderConfigListener(ImageFolderConfigWidget *widget, InputLoader::ImageFolderLoader *processor, QObject *parent);
	virtual void onSettingsChanged();
	virtual void onPropertyChanged(DesignNet::Property *prop);
	InputLoader::ImageFolderLoader*	m_processor;
	ImageFolderConfigWidget *		m_widget;

};

class ImageFolderConfigWidget : public DesignNet::ProcessorConfigWidget
{
	Q_OBJECT
	friend class ImageFolderConfigListener;
public:
	ImageFolderConfigWidget(DesignNet::Processor* processor, QWidget *parent);
	~ImageFolderConfigWidget();
	virtual ProcessorConfigWidget* create(DesignNet::Processor * processor, QWidget *parent = 0);
	QString getPath();

private:
	DesignNet::ProcessorConfigListener *m_listener;
	CustomUI::PathChooser *				m_pathChooser;
};

#endif // IMAGEFOLDERCONFIGWIDGET_H
