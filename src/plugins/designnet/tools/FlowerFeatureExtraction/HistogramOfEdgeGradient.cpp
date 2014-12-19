#include "HistogramOfEdgeGradient.h"
#include <qmath.h>
#include "../../../designnet/designnet_core/designnetbase/port.h"


using namespace DesignNet;
namespace FlowerFeatureExtraction{

enum PortIndex
{
	PortIndex_In_Binary,
	PortIndex_Out_HOEG_Features
};

static PortData s_ports[] =
{
	{ Port::IN_PORT,	DATATYPE_BINARYIMAGE,	"Binary Image" },
	{ Port::OUT_PORT,	DATATYPE_MATRIX,		"HOEG feature" }
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
	return tr("FlowerFeatureExtraction/Shape");
}

bool HistogramOfEdgeGradient::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	cv::Mat matBinary = getPortData<cv::Mat>(s_ports[PortIndex_In_Binary]);
	std::vector<std::vector<cv::Point> > countours;
	cv::findContours(matBinary.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double maxarea = 0;
	cv::Rect maxrect(0, 0, 2, 2);
	size_t max_index = -1;
	for (size_t i = 0; i < countours.size() ; ++i)
	{
		cv::Rect rect = cv::boundingRect(countours.at(i));
		if(rect.area() > maxarea)
		{
			maxarea = rect.area();
			maxrect = rect;
			max_index = i;
		}
	}
	const int iR = 10;
	std::vector<cv::Point> &max_countours = countours.at(max_index);
	std::vector<double> vecCounts;
	double fMaxArea = M_PI * iR * iR;
	for (size_t t = 0; t < max_countours.size(); t++)
	{
		cv::Point p = max_countours.at(t);
		cv::Rect rc(cv::Point(p.x - iR, p.y - iR), cv::Size(2 * iR, 2 * iR));
		int iCount = 0;
		for (int r = rc.y; r < rc.y + rc.height; r++)
		{
			for (int c = rc.x; c < rc.x + rc.width; c++)
			{
				if (r < 0 || c < 0 || r >= matBinary.rows || c >= matBinary.cols)
					continue;

				uchar u = matBinary.at<uchar>(r,c);
				int irr = sqrt(double(r - p.y) * (r - p.y) + (c - p.x) * (c - p.x));
				if (irr < iR && u == 255)
					iCount++;
			}
		}
		vecCounts.push_back(iCount / fMaxArea);
	}
	cv::Mat m = cv::Mat::zeros(matBinary.rows, matBinary.cols, CV_8UC1);
	cv::drawContours(m, countours, 0, cv::Scalar(255));
	cv::MatND hist;
	float hranges[] = { 0, 1 };
	const float *ranges[] = { hranges };
	int channels[] = { 0 };
	int hbins = 15;
	int histSize[] = { hbins };
	cv::Mat matCounts(vecCounts);
	matCounts.convertTo(matCounts, CV_32FC1);
	cv::calcHist(&matCounts, 1, channels, cv::Mat(),
		hist, 1, &hbins, ranges,
		true,
		false);
	hist = hist.reshape(1,1);
	cv::FileStorage fs("G:/hist.xml", cv::FileStorage::WRITE);
	fs << "hist" << hist;
	fs.release();
	pushData(qVariantFromValue(hist), DATATYPE_MATRIX, s_ports[PortIndex_Out_HOEG_Features].strName);
	notifyProcess();
	return true;
}

}
