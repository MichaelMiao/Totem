#include "ImageFolderLoader.h"
#include "property/pathdialogproperty.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/intdata.h"
#include <QIcon>
#include <QDir>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace DesignNet;
namespace InputLoader{

const char ImageFolderIcon[] = ":/InputLoader/images/add_folder.png";
ImageFolderLoader::ImageFolderLoader( DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/ )
	: Processor(space, parent),
	m_outPort(new ImageData(ImageData::IMAGE_BGR, this), Port::OUT_PORT),
	m_outImageCountPort(new IntData(0, this), Port::OUT_PORT)
{
	m_outPort.setName("ImageData");
	m_outImageCountPort.setName("outimagecount");
	m_outImageCountPort.data()->setPermanent(true);
	addPort(&m_outPort);
	addPort(&m_outImageCountPort);
	setName(tr("Image Folder Loader"));
	setIcon((QLatin1String(ImageFolderIcon)));
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
	if(m_folderPath.isEmpty())
	{
		emit logout(tr("Property %1 is not valid.").arg(m_folderPath));
		return false;
	}
	QStringList fileList;
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	QDir dir(m_folderPath);
	QFileInfoList infoList = dir.entryInfoList(nameFilters, 
		QDir::Files | QDir::Readable);
	foreach(QFileInfo info, infoList)
	{
		fileList << info.absoluteFilePath();
	}
	IntData intData(fileList.count());
	intData.setPermanent(true);
	pushData(&intData, "outimagecount");
	int i = 0;
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
			ImageData imageData;
			imageData.setImageData(mat);
			imageData.setIndex(i++);
			pushData(&imageData, "ImageData");
		}
	}
	return true;
}

void ImageFolderLoader::dataArrived( DesignNet::Port* port )
{
	Processor::dataArrived(port);
}

void ImageFolderLoader::setPath( const QString &p )
{
	QWriteLocker locker(&m_lock);
	m_folderPath = p;
}

QString ImageFolderLoader::path() const
{
	QReadLocker locker(&m_lock);
	return m_folderPath;
}

}//!< namespace InputLoader