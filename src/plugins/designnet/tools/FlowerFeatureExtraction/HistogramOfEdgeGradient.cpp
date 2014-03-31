#include "HistogramOfEdgeGradient.h"
#include "../../designnet_core/designnetbase/port.h"


namespace FlowerFeatureExtraction{

enum PortIndex
{
	PortIndex_In,
	PortIndex_Out,
};

static PortData s_ports[] =
{
	{ Port::OUT_PORT,	DATATYPE_MATRIX,	"Binary Image" },
};

HistogramOfEdgeGradient::HistogramOfEdgeGradient(DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/)
{
	setName(tr("HistogramOfEdgeGradient"));
	for (int i = 0; i < _countof(s_ports); i++)
		addPort(s_ports[i].ePortType, s_ports[i].eDataType, s_ports[i].strName);
}

HistogramOfEdgeGradient::~HistogramOfEdgeGradient()
{

}

QString HistogramOfEdgeGradient::title() const
{
	return tr("HistogramOfEdgeGradient");
}

QString HistogramOfEdgeGradient::category() const
{
	return tr("");
}

bool HistogramOfEdgeGradient::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	/// 求中心
	cv::Point2d centroid(0, 0);
	vector<vector<cv::Point> > countours;
	cv::findContours(m_binaryMat.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	size_t counts = countours.size();
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

	//////////////////////////////////////////////////////////////////////////
	/// 求重心 坐标求和后求平均值
	int iCount = 0;
	int iX = 0;
	int iY = 0;
	int iBottom = maxrect.y + maxrect.height;
	int iRight	= maxrect.x + maxrect.width;
	int iLeft	= maxrect.x;
	int iTop	= maxrect.y;
	for(int r = maxrect.y; r <= iBottom; ++r)
	{
		for(int c = maxrect.x; c <= iRight; ++c)
		{
			if (m_binaryMat.at<uchar>(r, c) == 255)
			{
				iX += c;
				iY += r;
				iCount++;
			}
		}
	}
	centroid.x = iX * 1.0 / iCount;
	centroid.y = iY * 1.0 / iCount;

	double radius = qMax(qMax(iRight - centroid.x, centroid.x - iLeft), qMax(centroid.y - iTop, iBottom - centroid.y));/// 半径
	float fFeature[4];
	memset(fFeature, 0, sizeof(float) * 4);
	vector<cv::Point> &vecPoints = countours.at(max_index);
	std::vector<float> vecD;
	float fSum = 0;
	for (size_t t = 0; t < vecPoints.size(); t++)
	{
		cv::Point dis = vecPoints.at(t) - cv::Point(centroid.x, centroid.y);
		float f = sqrt((double)dis.dot(dis));
		fSum += f;
		vecD.push_back(f);
	}
	fSum /= vecPoints.size();
	std::vector<int> vecKey;
	if (vecD.size() >= 2)
	{
		float fTemp = vecD.at(1) - vecD.at(0);
		float f = fTemp;
		float fMax = 0, fMin = 10000;
		int iPosLast = 0;
		for (size_t t = 2; t < vecD.size() - 1; t++)
		{
			fTemp = vecD.at(t) - vecD.at(t - 1);
			//			if (f < 0 && fTemp > 0 || (f > 0 && fTemp < 0) && fTemp > 0)
			if (vecD.at(t) < vecD.at(t - 1) && vecD.at(t) < vecD.at(t + 1))
			{
				if (fMax < vecD.at(t))
					fMax = vecD.at(t);
				else if (fMin > vecD.at(t))
					fMin = vecD.at(t);
				iPosLast = t;
				vecKey.push_back(t);
				f = fTemp;
			}
		}
		float fThreshold = (fMax + fMin) / 2;
		std::vector<int> goodKey;
		for (size_t t = 1; t < vecKey.size(); t++)
		{
			if (vecD.at(vecKey.at(t)) < fThreshold)
			{
				goodKey.push_back(vecKey.at(t));
			}
		}
		int iCount = goodKey.size();
		for (size_t t = 0; t < goodKey.size(); t++)
		{
			cv::Rect rc(vecPoints.at(goodKey.at(t)).x, vecPoints.at(goodKey.at(t)).y, 10, 10);
			cv::rectangle(m_srcMat, rc, cv::Scalar(255, 0, 0));
		}
	}

	cv::Mat mat;
	for (int i = 0; i < 4; i++)
	{
		mat.push_back(fFeature[i]);
	}
	mat.reshape(1, 1);
	return mat;
	return true;
}

}
