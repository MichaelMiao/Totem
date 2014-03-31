#include "centroid.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "../../../designnet/designnet_core/data/imagedata.h"
#include "../../../designnet/designnet_core/data/matrixdata.h"
#include "../../../designnet/designnet_core/designnetbase/port.h"
#include "../../../designnet/designnet_core/property/optionproperty.h"

#define INPUT_GRAYIMAGE		"grayInputImage"
#define OUTPUT_CENTROID		"Centroid"

using namespace std;
using namespace DesignNet;


namespace FeatureExtraction{
Centroid::Centroid(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent)
{
	addPort(Port::IN_PORT, DATATYPE_BINARYIMAGE, INPUT_GRAYIMAGE);
	addPort(Port::OUT_PORT, DATATYPE_MATRIX, OUTPUT_CENTROID);
	setName(tr("Centroid"));
}

QString Centroid::title() const
{
	return tr("Centroid");
}

QString Centroid::category() const
{
	return tr("FeatureExtraction");
}

bool Centroid::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	notifyDataWillChange();
	cv::Mat binaryImage = ((MatrixData*)getData("grayImageInput").at(0))->getMatrix();
	/// ������
	cv::Point2d centroid(0, 0);
	vector<vector<cv::Point> > countours;
	cv::findContours(binaryImage.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
	if(counts <= 0)
		return false;/// �ҵ�������������Ϊ0
	double maxarea = 0;
	cv::Rect maxrect(0, 0, 2, 2);
	size_t max_index = -1;
	for (size_t i = 0; i < counts ; ++i)
	{
		cv::Rect rect = cv::boundingRect(countours.at(i));
		if(rect.area() > maxarea)
		{
			maxarea = rect.area();
			maxrect = rect;
			max_index = i;
		}
	}

	/// ������ ������ͺ���ƽ��ֵ
	int iCount = 0;
	int iX = 0;
	int iY = 0;
	int iBottom = maxrect.y + maxrect.height;
	int iRight = maxrect.x + maxrect.width;
	for(int r = maxrect.y; r < iBottom; ++r)
	{
		for(int c = maxrect.x; c < iRight; ++c)
		{
			if (binaryImage.at<uchar>(r, c) == 255)
			{
				iX += c;
				iY += r;
				iCount++;
			}
		}
	}
	centroid.x = iX / iCount;
	centroid.y = iY / iCount;
	cv::Mat mat(1, 2, CV_32FC1);
	mat.at<float>(0, 0) = centroid.x;
	mat.at<float>(0, 1) = centroid.y;
	MatrixData data;
	data.setMatrix(mat);
	pushData(&data, "centroid");
	notifyProcess();
	return true;
}

void Centroid::propertyChanged( DesignNet::Property *prop )
{

}



}
