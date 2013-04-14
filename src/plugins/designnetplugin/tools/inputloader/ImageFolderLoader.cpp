#include "..\..\..\..\shared\totem_gui_pch.h"
#include "ImageFolderLoader.h"
#include "property/pathdialogproperty.h"

#include <QIcon>
#include <QDir>

using namespace DesignNet;
namespace InputLoader{

const char ImageFolderIcon[] = ":/InputLoader/images/add_folder.png";
ImageFolderLoader::ImageFolderLoader( DesignNet::DesignNetSpace *space, QGraphicsItem *parent /*= 0*/ )
	: ProcessorGraphicsBlock(space, parent),
	m_outPort(Port::OUT_PORT)
{
	m_outPort.setName("ImageData");
	addPort(&m_outPort);
	

	m_pathProperty = new PathDialogProperty(tr("OpenFile"),
		QDir::currentPath(),
		QStringList(),
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
	return true;
}

void ImageFolderLoader::dataArrived( DesignNet::Port* port )
{

}

}//!< namespace InputLoader