#include "ImageFolderLoader.h"
#include "property/pathdialogproperty.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include <QIcon>
#include <QDir>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace DesignNet;
namespace InputLoader{

const char ImageFolderIcon[] = ":/InputLoader/images/add_folder.png";
ImageFolderLoader::ImageFolderLoader( DesignNet::DesignNetSpace *space, QGraphicsItem *parent /*= 0*/ )
	: ProcessorGraphicsBlock(space, parent),
	m_outPort(new ImageData(ImageData::IMAGE_BGR, this), Port::OUT_PORT)
{
	m_outPort.setName("ImageData");
	addPort(&m_outPort);
	
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	m_pathProperty = new PathDialogProperty(tr("OpenDirectory"),
		QDir::currentPath(),
		nameFilters,
		QDir::Dirs, true, this);
	m_pathProperty->setObjectName(tr("Image Folder"));
	addProperty(m_pathProperty);
	setName(tr("Image Folder Loader"));
	setIcon(QIcon(QLatin1String(ImageFolderIcon)));
}


ImageFolderLoader::~ImageFolderLoader(void)
{
}

Processor* ImageFolderLoader::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new ImageFolderLoader(space);
}

QString ImageFolderLoader::title() const
{
	return tr("Image Folder Loader");
}

QString ImageFolderLoader::category() const
{
	return tr("Loader");
}

bool ImageFolderLoader::process()
{
	emit logout(tr("ImageFolderLoader Processing"));
	if(!m_pathProperty->isValid())
	{
		emit logout(tr("Property %1 is not valid.").arg(m_pathProperty->name()));
		return false;
	}
	QList<Utils::Path> paths = m_pathProperty->paths();
	if(paths.count() > 0)
	{
		foreach(Utils::Path path, paths)
		{
			QDir dir(path.m_path);
			QStringList fileList = dir.entryList(m_pathProperty->nameFilters(), QDir::Files | QDir::Readable);
			foreach(QString file, fileList)
			{
				std::string str = dir.absoluteFilePath(file).toLocal8Bit().data();
				cv::Mat mat = cv::imread(str);
				if(!mat.data)
				{
					emit logout(tr("Image Loader %1 load %2 failed").arg(id()).arg(file));
					return false;
				}
				else
				{
					emit logout(tr("Image Loader id:%1 is loading %2...").arg(id()).arg(file));
					ImageData imageData;
					imageData.setImageData(mat);
					pushData(&imageData, "ImageData");
				}
			}
		}
		
	}
	return true;
}

void ImageFolderLoader::dataArrived( DesignNet::Port* port )
{
	ProcessorGraphicsBlock::dataArrived(port);
}

}//!< namespace InputLoader