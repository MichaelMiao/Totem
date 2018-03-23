#ifndef OPENCVHELPER_H
#define OPENCVHELPER_H


#include "utils_global.h"
#ifdef USE_OPEN_CV
#include <opencv2/core/core.hpp>
#include <QImage>
namespace Utils{

class TOTEM_UTILS_EXPORT OpenCVHelper
{
public:
    OpenCVHelper();
    static QImage Mat2QImage(const cv::Mat& mat);
};
}
#endif 

#endif // OPENCVHELPER_H
