#ifndef IMAGEDATA_H
#define IMAGEDATA_H

#include "idata.h"
#include "opencv2/core/core.hpp"
namespace DesignNet {
class ImageDataWidget;
class DESIGNNET_CORE_EXPORT ImageData : public IData
{
    Q_OBJECT
    friend class ImageDataWidget;
public:
    explicit ImageData(QObject *parent = 0);
    virtual Core::Id id();
    void setImageData(const cv::Mat& mat);
    cv::Mat imageData() const;
signals:

public slots:
protected:
    cv::Mat m_imageMat;//!< opencv Image图像
};
}

#endif // IMAGEDATA_H
