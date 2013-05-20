#include "histogramdata.h"
#include "designnetconstants.h"
namespace DesignNet{

HistogramData::HistogramData(QObject *parent)
	: IData(parent)
{

}

HistogramData::~HistogramData()
{

}

Core::Id HistogramData::id()
{
	return Constants::DATA_TYPE_HISTOGRAM;
}

IData* HistogramData::clone( QObject *parent /*= 0*/ )
{
	HistogramData *data = new HistogramData(parent);
	m_hist.copyTo(data->m_hist);
	return data;
}

bool HistogramData::copy( IData* data )
{
	if(data->id() != this->id())/// 数据类型要一致
	{
		return false;
	}
}

void HistogramData::setHistogram( const cv::Mat &hist )
{
	hist.copyTo(this->m_hist);
	emit dataChanged();
}

bool HistogramData::isValid() const
{
	return true;
}

QImage HistogramData::image()
{
	return m_image;
}

}
