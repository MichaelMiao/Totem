#include "imagedata.h"
#include "designnetconstants.h"
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
}
