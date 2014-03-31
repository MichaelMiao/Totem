#ifndef OPENCVHELPER_H
#define OPENCVHELPER_H


#include "utils_global.h"
#include <opencv2/core/core.hpp>
#include <QImage>
namespace Utils{

class TOTEM_UTILS_EXPORT OpenCVHelper
{
public:
    OpenCVHelper();
    static QImage Mat2QImage(const cv::Mat& mat);
	static cv::Mat QImage2Mat(QImage img);
//	static cv::Mat Combine(cv::Mat &mat1, cv::Mat& mat2);

};
}

#endif // OPENCVHELPER_H
