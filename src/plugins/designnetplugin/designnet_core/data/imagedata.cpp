#include "imagedata.h"
#include "designnetconstants.h"
#include "Utils/totemassert.h"
namespace DesignNet{

ImageData::ImageData(QObject *parent) :
    IData(parent)
{
}

Core::Id ImageData::id()
{
    return DesignNet::Constants::DATA_TYPE_IMAGE;
}

void ImageData::setImageData(const cv::Mat &mat)
{
    m_imageMat = mat;
    emit dataChanged();
}

cv::Mat ImageData::imageData() const
{
    return m_imageMat;
}

IData* ImageData::clone( QObject *parent /*= 0 */ )
{
	ImageData *imageData = new ImageData(parent);
	imageData->setImageData(this->imageData().clone());
	return imageData;
}

bool ImageData::copy( IData* data )
{
	TOTEM_ASSERT(data->id() == id(), return false);
	ImageData *imageData = qobject_cast<ImageData*>(data);
	if(!imageData)
	{
		return false;
	}
	m_imageMat = imageData->imageData().clone();
	emit dataChanged();
	return true;

}

}
