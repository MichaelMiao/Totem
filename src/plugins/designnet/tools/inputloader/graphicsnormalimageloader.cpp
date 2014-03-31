#include "stdafx.h"
#include "graphicsnormalimageloader.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <QDebug>
#include "../../../coreplugin/icore.h"
#include "../../../coreplugin/messagemanager.h"
#include "../../../designnet/designnet_core/designnetbase/processor.h"
#include "../../../designnet/designnet_core/property/pathdialogproperty.h"
#include "../../../designnet/designnet_core/property/property.h"


using namespace std;
using namespace DesignNet;

namespace InputLoader{

const char NormalImageLoader[] = ":/InputLoader/images/NormalImageLoader.png";
const char PROPERTY_IMAGEFILE[] = "ImageFilePath";


GraphicsNormalImageLoader::GraphicsNormalImageLoader(DesignNetSpace *space, QObject* parent)
    : Processor(space, parent)
{
    m_name = tr("NormalImageLoader");
    addPort(Port::OUT_PORT, DATATYPE_8UC3IMAGE, tr("ImageData"));
    setIcon(QLatin1String(NormalImageLoader));
	QStringList nameFilters;
	nameFilters << "*.jpg" << "*.bmp";
	PathDialogProperty *pProperty = new PathDialogProperty(PROPERTY_IMAGEFILE, "", nameFilters,
		QDir::Files, true, this);
	addProperty(pProperty);
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

bool GraphicsNormalImageLoader::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
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
		notifyDataWillChange();
		pushData(qVariantFromValue(mat), DATATYPE_8UC3IMAGE, "ImageData");
		notifyProcess();
	}

    return true;
}

void GraphicsNormalImageLoader::propertyChanged( Property *prop )
{
}

void GraphicsNormalImageLoader::setPath(const QString &f)
{
	QWriteLocker locker(&m_lock);
	m_filePath = f;
	PathDialogProperty * pProperty = (PathDialogProperty *)getProperty(PROPERTY_IMAGEFILE);
	QList<Utils::Path> paths;
	Utils::Path p;
	p.m_path = m_filePath;
	paths << p;
	pProperty->setPaths(paths);
}

QString GraphicsNormalImageLoader::path() const
{
	QReadLocker locker(&m_lock);
	return m_filePath;
}

}
