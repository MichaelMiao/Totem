#include "centroid.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/matrixdata.h"
#include "designnet/designnet_core/property/optionproperty.h"
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace DesignNet;

namespace FeatureExtraction{
class CentroidPrivate
{
public:
	CentroidPrivate(Centroid *centroid)
	{
		m_inputBinaryImagePort = new DesignNet::Port(new ImageData(ImageData::IMAGE_BINARY, 0), Port::IN_PORT, "grayImageInput", centroid);
		m_outputFeaturePort	= new DesignNet::Port(new MatrixData(0), Port::OUT_PORT, "centroid", centroid);
	}
	~CentroidPrivate()
	{

	}
	DesignNet::Port *m_inputBinaryImagePort;
	DesignNet::Port *m_outputFeaturePort;
};
Centroid::Centroid(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent),
	d(new CentroidPrivate(this))
{
	addPort(d->m_inputBinaryImagePort);
	addPort(d->m_outputFeaturePort);
	setName(tr("Centroid"));
}

Centroid::~Centroid()
{
	delete d;
}

Processor* Centroid::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new Centroid(space);
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
	cv::Mat binaryImage = ((MatrixData*)getData("grayImageInput").at(0))->getMatrix();
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
	centroid.x = iX / iCount;
	centroid.y = iY / iCount;
	cv::Mat mat(1, 2, CV_32FC1);
	mat.at<float>(0, 0) = centroid.x;
	mat.at<float>(0, 1) = centroid.y;
	MatrixData data;
	data.setMatrix(mat);
	pushData(&data, "centroid");
	return true;
}

void Centroid::propertyChanged( DesignNet::Property *prop )
{

}



}

