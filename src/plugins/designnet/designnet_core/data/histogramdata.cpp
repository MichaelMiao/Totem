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
	data->setIndex(m_index);
	return data;
}

bool HistogramData::copy( IData* data )
{
	if(data->id() != this->id())/// 数据类型要一致
	{
		return false;
	}
	HistogramData* hd = (HistogramData*)(data);
	hd->histogram().copyTo(m_hist);
	
	return IData::copy(data);;
}

void HistogramData::setHistogram( const cv::Mat &hist )
{
	hist.copyTo(this->m_hist);
	emit dataChanged();
}

bool HistogramData::isValid() const
{
	if (m_hist.rows > 0 && m_hist.cols > 0)
	{
		return true;
	}
	return false;
}

QImage HistogramData::image()
{
	return m_image;
}

cv::Mat HistogramData::histogram()
{
	return m_hist;
}

}
