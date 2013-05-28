#include "ImageFolderLoader.h"
#include "property/pathdialogproperty.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include "designnetplugin/designnet_core/data/intdata.h"
#include <QIcon>
#include <QDir>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace DesignNet;
namespace InputLoader{

const char ImageFolderIcon[] = ":/InputLoader/images/add_folder.png";
ImageFolderLoader::ImageFolderLoader( DesignNet::DesignNetSpace *space, QGraphicsItem *parent /*= 0*/ )
	: ProcessorGraphicsBlock(space, parent),
	m_outPort(new ImageData(ImageData::IMAGE_BGR, this), Port::OUT_PORT),
	m_outImageCountPort(new IntData(0, this), Port::OUT_PORT)
{
	m_outPort.setName("ImageData");
	m_outImageCountPort.setName("outimagecount");
	addPort(&m_outPort);
	addPort(&m_outImageCountPort);
	
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
		QStringList fileList;
		foreach(Utils::Path path, paths)
		{
			QDir dir(path.m_path);
			QFileInfoList infoList = dir.entryInfoList(m_pathProperty->nameFilters(), QDir::Files | QDir::Readable);
			foreach(QFileInfo info, infoList)
			{
				fileList << info.absoluteFilePath();
			}
		}
		IntData intData(fileList.count());
		pushData(&intData, "outimagecount");
		foreach(QString file, fileList)
		{
			std::string str = file.toLocal8Bit().data();
			cv::Mat mat = cv::imread(str);
			if(!mat.data)
			{
				emit logout(tr("load %1 failed").arg(file));
				return false;
			}
			else
			{
				emit logout(tr("loading %1...").arg(file));
				qDebug() << "miao"<<file;
				ImageData imageData;
				imageData.setImageData(mat);
				pushData(&imageData, "ImageData");
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