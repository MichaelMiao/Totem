#include "imagedata.h"
#include "designnetconstants.h"
#include "Utils/totemassert.h"
namespace DesignNet{

ImageData::ImageData(int type, QObject *parent) :
    IData(parent),
	m_type(type)
{
	switch (m_type){
	case IMAGE_BGR:
		{
			m_image.load(QLatin1String(Constants::DATA_IMAGE_COLOR_IMAGE));
			break;
		}
	case IMAGE_GRAY:
		{
			m_image.load(QLatin1String(Constants::DATA_IMAGE_GRAY_IMAGE));
			break;
		}
	case IMAGE_BINARY:
		{
			m_image.load(QLatin1String(Constants::DATA_IMAGE_BINARY_IMAGE));
			break;
		}
	}
	
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
	ImageData *imageData = new ImageData(this->imageType(), parent);
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
	imageData->imageData().copyTo(m_imageMat);
	emit dataChanged();
	return true;

}

bool ImageData::isValid() const
{
	if (m_imageMat.empty())
	{
		return false;
	}
	return true;
}

QImage ImageData::image()
{
	return m_image;
}

}
