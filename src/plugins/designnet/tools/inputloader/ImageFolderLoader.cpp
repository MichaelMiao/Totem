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
	m_iCurIndex(0)
{
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

bool ImageFolderLoader::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	emit logout(tr("ImageFolderLoader Processing"));
	ProcessResult pr;
	if (m_iCurIndex >= m_filePaths.size())
	{
		pr.m_bSucessed = true;
		pr.m_bNeedLoop = false;
		future.reportResult(pr);
		return true;
	}
	pr.m_bNeedLoop = true;
	QString file = m_filePaths.at(m_iCurIndex);
	std::string str = file.toLocal8Bit().data();
	cv::Mat mat = cv::imread(str);
	if(!mat.data)
	{
		emit logout(tr("load %1 failed").arg(file));
		pr.m_bSucessed = false;
		pr.m_bNeedLoop = false;
		future.reportResult(pr);
		return false;
	}
	else
	{
		emit logout(tr("loading %1...").arg(file));
		m_imageData.setImageData(mat);
		m_imageData.setIndex(m_iCurIndex++);
		pushData(QVariant::fromValue((IData*)&m_imageData), "ImageData");
	}
	future.reportResult(pr);
	return true;
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

bool ImageFolderLoader::prepareProcess()
{
	m_folderPath = QString::fromUtf8("F:\\MD\\flower\\Test Data\\");
	if(m_folderPath.isEmpty())
	{
		emit logout(tr("Property %1 is not valid.").arg(m_folderPath));
		return false;
	}
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	QDir dir(m_folderPath);
	QFileInfoList infoList = dir.entryInfoList(nameFilters, 
		QDir::Files | QDir::Readable);
	
	foreach(QFileInfo info, infoList)
		m_filePaths << info.absoluteFilePath();

	pushData(m_filePaths.count(), "outimagecount");

	return true;
}

bool ImageFolderLoader::finishProcess()
{
	m_iCurIndex = 0;
	return true;
}

}//!< namespace InputLoader