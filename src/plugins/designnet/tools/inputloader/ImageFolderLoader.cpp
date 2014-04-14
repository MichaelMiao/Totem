#include "stdafx.h"
#include "ImageFolderLoader.h"
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QMovie>
#include <QMultiMap>
#include <QTextStream>
#include "../../../designnet/designnet_core/data/imagedata.h"
#include "../../../designnet/designnet_core/data/intdata.h"
#include "../../../designnet/designnet_core/property/pathdialogproperty.h"
#include "../../designnet_core/designnetconstants.h"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/ocl.hpp"
#include "opencv2/ocl/ocl.hpp"
#include "Utils/fileutils.h"
#include "Utils/opencvhelper.h"


using namespace DesignNet;
#define OUTPUT_IMAGE			_T("Output Image")
#define OUTPUT_FILE_PATH		_T("Image File Path")
#define OUTPUT_DATA_INDEX		_T("Index")

namespace InputLoader{

const char ImageFolderIcon[] = ":/InputLoader/images/add_folder.png";
ImageFolderLoader::ImageFolderLoader(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent, ProcessorType_Permanent),
	m_iCurIndex(0)
{
	setName(tr("Image Folder Loader"));
	setIcon(QLatin1String(ImageFolderIcon));
	addPort(Port::OUT_PORT, DATATYPE_8UC3IMAGE, OUTPUT_IMAGE);
	addPort(Port::OUT_PORT, DATATYPE_STRING, OUTPUT_FILE_PATH);
	addPort(Port::OUT_PORT, DATATYPE_INT, OUTPUT_DATA_INDEX);
	QStringList fileTypes;
	PathDialogProperty* pProperty = new PathDialogProperty("Path", "I:/data/_train", fileTypes, QDir::Dirs, true, this);
	addProperty(pProperty);
}

ImageFolderLoader::~ImageFolderLoader(void)
{
	m_waitLock.unlock();
}

QString ImageFolderLoader::title() const
{
	return tr("Image Folder Loader");
}

QString ImageFolderLoader::category() const
{
	return tr("Loader");
}

bool ImageFolderLoader::prepareProcess()
{
	m_waitLock.tryLock();
	m_waitLock.unlock();
	m_iCurIndex = 0;
	PathDialogProperty *path = (PathDialogProperty*)getProperty("Path");
	if(path->paths().size() <= 0)
	{
		emit logout(tr("Image Folder Path is invalid."));
		return false;
	}
	QString folderPath = path->paths().at(0).m_path;
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	QDir dir(folderPath);
	QFileInfoList infoList = dir.entryInfoList(nameFilters, 
		QDir::Files | QDir::Readable);
	
	m_filePaths.clear();
	foreach(QFileInfo info, infoList)
		m_filePaths << info.absoluteFilePath();
	return true;
}

bool ImageFolderLoader::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	emit logout(tr("ImageFolderLoader Processing"));
	waitForAccept();
	foreach (QString file, m_filePaths)
	{
		notifyDataWillChange();
		std::string str = file.toLocal8Bit().data();
		qDebug() << file;
		cv::Mat mat = cv::imread(str);
		if(!mat.data)
		{
			emit logout(tr("load %1 failed").arg(file));
			return false;
		}
		else
		{
			emit logout(tr("loading %1...").arg(file));
			ProcessData pd;
			
			pd.m_iIndex = m_iCurIndex++;
			pd.variant.setValue(mat);
			DataType dt = DATATYPE_MATRIX;
			if (mat.type() == CV_8UC1)
				dt = DATATYPE_GRAYIMAGE;
			else if (mat.type() == CV_8UC3)
				dt = DATATYPE_8UC3IMAGE;
			pd.dataType = dt;
			pushData(pd, OUTPUT_IMAGE);
			if (m_iCurIndex >= m_filePaths.size())
				m_iCurIndex = -1;
			pushData(qVariantFromValue(m_iCurIndex), DATATYPE_INT, OUTPUT_DATA_INDEX);
			pushData(qVariantFromValue(file), DATATYPE_STRING, OUTPUT_FILE_PATH);
		}
		notifyProcess();
		if (getOutputProcessor().size() > 0)
			waitForAccept();
	}
	return true;
}

bool ImageFolderLoader::finishProcess()
{
	return true;
}

bool ImageFolderLoader::waitForAccept()
{
	m_waitLock.lock();
	m_listWaitProcessors = getOutputProcessor();
	return true;
}

void ImageFolderLoader::onChildProcessorFinish(Processor* p)
{
	m_listWaitProcessors.removeOne(p);
	if (m_listWaitProcessors.empty())
		m_waitLock.unlock();
}

}//!< namespace InputLoader
