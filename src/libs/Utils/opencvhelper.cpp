#include "opencvhelper.h"
#include <QDebug>
namespace Utils {

OpenCVHelper::OpenCVHelper()
{
}

QImage OpenCVHelper::Mat2QImage(const cv::Mat &mat)
{
    if(mat.type()==CV_8UC1)
    {
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++)
            colorTable.push_back(qRgb(i,i,i));

        const uchar *qImageBuffer = (const uchar*)mat.data;

        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }

    if(mat.type()==CV_8UC3)
    {

        const uchar *qImageBuffer = (const uchar*)mat.data;
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    qDebug() << "ERROR: Mat could not be converted to QImage.";
    return QImage();
}

cv::Mat OpenCVHelper::QImage2Mat(QImage img)
{
	cv::Mat mat = cv::Mat(img.height(), img.width(), CV_8UC4, (uchar*)img.bits(), img.bytesPerLine()); 
	cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3); 
	int from_to[] = { 0, 0, 1, 1, 2, 2 }; 
	cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 ); 
	return mat2; 
}

// cv::Mat OpenCVHelper::Combine(cv::Mat &mat1, cv::Mat& mat2)
// {
// 	if (mat1.rows != mat2.rows)
// 		return cv::Mat();
// 
// 	cv::Mat m(mat1.rows, mat1.cols + mat2.cols, mat1.type());
// 	return m;
// }

}
