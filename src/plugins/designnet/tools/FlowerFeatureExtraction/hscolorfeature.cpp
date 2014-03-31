#include "hscolorfeature.h"
#include <opencv2/imgproc/imgproc.hpp>
#include "../../../designnet/designnet_core/data/imagedata.h"
#include "../../../designnet/designnet_core/data/matrixdata.h"
#include "../../../designnet/designnet_core/designnetbase/port.h"
#include "../../designnet_core/data/histogramdata.h"
#include "../../designnet_core/data/idata.h"
#include "designnet/designnet_core/graphicsitem/portitem.h"


using namespace DesignNet;
namespace FlowerFeatureExtraction{

static int compareDis(std::pair<cv::Point2i, float> p1, std::pair<cv::Point2i, float> p2)
{
	return p1.second > p2.second;
}

HSColorFeature::HSColorFeature(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent)
{
	m_featureData = new DesignNet::MatrixData(this);
	addPort(Port::IN_PORT, DATATYPE_8UC3IMAGE, "HSV Color Image");
	addPort(Port::OUT_PORT, DATATYPE_HISTOGRAM, "Matrix Histogram");
	addPort(Port::IN_PORT, DATATYPE_BINARYIMAGE, "Binary Color Image");
	setName(tr("Color Feature HSV"));
}

HSColorFeature::~HSColorFeature()
{

}

QString HSColorFeature::title() const
{
	return tr("Color Histogram HSV");
}

QString HSColorFeature::category() const
{
	return tr("Flowers/Feature");
}

bool HSColorFeature::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	MatrixData* pMatrix = (MatrixData*)getOneData("HSV Color Image").variant.value<IData*>();
	HistogramData* pHistogram = (HistogramData*)getOneData("Binary Color Image").variant.value<IData*>();
	cv::Mat hsvMat = pMatrix->getMatrix();
	cv::Mat hist = pHistogram->histogram();
	notifyDataWillChange();
	cv::Mat mat = extractColor(hsvMat, hist);
	QVariant::fromValue(mat);
	notifyProcess();
	return true;
}

void HSColorFeature::propertyChanged( DesignNet::Property *prop )
{

}

cv::Mat HSColorFeature::extractColor(cv::Mat& hsvMat, cv::Mat& binaryMat)
{
	cv::Mat matRet;
	int hbins = 40, sbins = 50;
	int histSize[] = { hbins, sbins };
	float hranges[] = { 0, 180 };
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	cv::MatND hist;
	int channels[] = { 0, 1 };
	cv::calcHist(&hsvMat, 1, channels, binaryMat,
		hist, 2, histSize, ranges,
		true, // the histogram is uniform
		false );

	double maxVal=0;
	cv::minMaxLoc(hist, 0, &maxVal, 0, 0);

	int scale = 10;
	cv::Mat histImg = cv::Mat::zeros(sbins * scale, hbins * 10, CV_8UC3);
	std::vector<std::pair<cv::Point2i, float> > fVec;
	for( int h = 0; h < hbins; h++ )
	{
		for( int s = 0; s < sbins; s++ )
			fVec.push_back(std::make_pair(cv::Point2i(h, s), hist.at<float>(h, s)));
	}
	sort(fVec.begin(), fVec.end(), compareDis);
	for (int i = 0; i < 64; i++)
	{
		matRet.push_back(fVec.at(i).first.x);
		matRet.push_back(fVec.at(i).first.y);
	}
	matRet = matRet.reshape(1, 1);
	matRet.convertTo(matRet, CV_32F);
	return matRet;
}

}
