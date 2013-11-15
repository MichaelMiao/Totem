#include "graphicsnormalimageloader.h"
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/property/pathdialogproperty.h"
#include "designnet/designnet_core/property/property.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "coreplugin/icore.h"
#include "coreplugin/messagemanager.h"
#include <iostream>

#include <QDebug>
using namespace std;
using namespace DesignNet;
namespace InputLoader{
const char NormalImageLoader[] = ":/InputLoader/images/NormalImageLoader.png";
GraphicsNormalImageLoader::GraphicsNormalImageLoader(DesignNetSpace *space, QObject* parent)
    : Processor(space, parent),
      m_outPort(new ImageData(ImageData::IMAGE_BGR, this), Port::OUT_PORT)
{
    m_name = this->metaObject()->className();
    m_outPort.setName("ImageData");
    addPort(&m_outPort);
    m_imageData = new ImageData(ImageData::IMAGE_BGR, this);
    setName(tr("Image File"));
    setIcon(QLatin1String(NormalImageLoader));
}

Processor *GraphicsNormalImageLoader::create(DesignNetSpace *space) const
{
    return new GraphicsNormalImageLoader(space);
}

QString GraphicsNormalImageLoader::title() const
{
    return name();
}

QString GraphicsNormalImageLoader::category() const
{
    return tr("Loader");
}

bool GraphicsNormalImageLoader::process()
{
    emit logout(tr("GraphicsNormalImageLoader Processing"));
	if(m_filePath.isEmpty())
	{
		emit logout(tr("Property %1 is not valid.").arg(m_filePath));
		return false;
	}
    
	QString path = m_filePath;
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
		m_imageData->setIndex(0);
		pushData(m_imageData, "ImageData");
	}

    return true;
}

void GraphicsNormalImageLoader::propertyChanged( Property *prop )
{
}

void GraphicsNormalImageLoader::setPath( const QString &p )
{
	QWriteLocker locker(&m_lock);
	m_filePath = p;
}

QString GraphicsNormalImageLoader::path() const
{
	QReadLocker locker(&m_lock);
	return m_filePath;
}

}
