#include "centroid.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include "../../../designnet/designnet_core/data/imagedata.h"
#include "../../../designnet/designnet_core/data/matrixdata.h"
#include "../../../designnet/designnet_core/designnetbase/port.h"
#include "../../../designnet/designnet_core/property/optionproperty.h"


using namespace std;
using namespace DesignNet;
enum PortIndex
{
	PortIndex_In_BinaryImage,
	PortIndex_Out_Centroid,
};

static PortData s_ports[] =
{
	{ Port::IN_PORT,	DATATYPE_BINARYIMAGE,		"Gray Image" },
	{ Port::OUT_PORT,	DATATYPE_MATRIX,			"Centroid" },
};


namespace FeatureExtraction{
Centroid::Centroid(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent)
{
	for (int i = 0; i < _countof(s_ports); i++)
		addPort(s_ports[i].ePortType, s_ports[i].eDataType, s_ports[i].strName);
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
	cv::Mat binaryImage = getPortData<cv::Mat>(s_ports[PortIndex_In_BinaryImage]);
	/// 求中心
	cv::Point2d centroid(0, 0);
	vector<vector<cv::Point> > countours;
	cv::findContours(binaryImage.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
	if(counts <= 0)
		return false;/// 找到的轮廓数不能为0
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

	/// 求重心 坐标求和后求平均值
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
	centroid.x = iX * 1.0 / iCount;
	centroid.y = iY * 1.0 / iCount;
	cv::Mat mat(1, 2, CV_32FC1);
	mat.at<float>(0, 0) = centroid.x;
	mat.at<float>(0, 1) = centroid.y;
	pushData(qVariantFromValue(mat), DATATYPE_MATRIX, s_ports[PortIndex_Out_Centroid].strName);
	notifyProcess();
	return true;
}

void Centroid::propertyChanged( DesignNet::Property *prop )
{

}



}
