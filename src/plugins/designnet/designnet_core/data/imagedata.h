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
	enum{
		IMAGE_GRAY,
		IMAGE_BINARY,
		IMAGE_BGR,
		IMAGE_BGRA
	};

	explicit ImageData(int type = IMAGE_BGR, QObject *parent = 0);//!< type可以是IMAGE_GRAY,IMAGE_BINARY,IMAGE_BGR,IMAGE_BGRA
    virtual Core::Id id();
    void setImageData(const cv::Mat& mat);
    cv::Mat imageData() const;
	IData* clone(QObject *parent = 0 );
	virtual bool copy(IData* data);
	virtual bool isValid() const;
	virtual QImage image();//!< 类型图片
	int imageType() const{return m_type;}
signals:

public slots:
protected:
    cv::Mat m_imageMat;//!< opencv Image图像
	int m_type;
};
}

#endif // IMAGEDATA_H
