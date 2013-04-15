#include "graphicsnormalimageloader.h"
#include "designnetplugin/designnet_core/designnetbase/processor.h"
#include "designnetplugin/designnet_core/property/pathdialogproperty.h"
#include "designnetplugin/designnet_core/property/property.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDebug>
using namespace std;
using namespace DesignNet;
namespace InputLoader{
const char NormalImageLoader[] = ":/InputLoader/images/NormalImageLoader.png";
GraphicsNormalImageLoader::GraphicsNormalImageLoader(DesignNetSpace *space, QGraphicsItem *parent)
    : ProcessorGraphicsBlock(space, parent),
      m_outPort(Port::OUT_PORT)
{
    m_name = this->metaObject()->className();
    m_outPort.setName("ImageData");
    addPort(&m_outPort);
    QStringList nameFilters;
    nameFilters << "Images (*.bmp *.xpm *.jpg)";

    m_imageFile = new PathDialogProperty("OpenFile",
                                         QDir::currentPath(),
                                         nameFilters,
                                         QDir::Files, true, this);
    m_imageFile->setObjectName(tr("Image File"));
    addProperty(m_imageFile);
    m_imageData = new ImageData(this);
    setName(tr("Image File"));
    setIcon(QIcon(QLatin1String(NormalImageLoader)));
}

Processor *GraphicsNormalImageLoader::create(DesignNetSpace *space) const
{
    return new GraphicsNormalImageLoader(space, parentItem());
}

QString GraphicsNormalImageLoader::title() const
{
    return name();
}

QString GraphicsNormalImageLoader::category() const
{
    return tr("Loader");
}

bool GraphicsNormalImageLoader::process(QFutureInterface<bool> &fi)
{
    emit logout(tr("GraphicsNormalImageLoader Process"));
    QList<Utils::Path> paths = m_imageFile->paths();
    if(paths.count() > 0)
    {
        QString path = paths.at(0).m_path;
		std::string str = path.toLocal8Bit().data();
		cv::Mat mat = cv::imread(str);
		if(!mat.data)
		{
			emit logout(tr("Image Loader %1 load %2 failed").arg(id()).arg(path));
			return false;
		}
		else
		{
			m_imageData->setImageData(mat);
			pushData(m_imageData, "ImageData");
		}
		
    }

    return true;
}

void GraphicsNormalImageLoader::dataArrived(Port *port)
{
	
}

void GraphicsNormalImageLoader::propertyChanged( Property *prop )
{
	if(m_imageFile->isValid())
	{
		emit logout(tr("Property %1 data is ready").arg(prop->name()));
		setDataReady(true);
	}
}

}
