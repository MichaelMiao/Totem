#ifndef HISTOGRAMDATA_H
#define HISTOGRAMDATA_H

#include "idata.h"
#include <QObject>
#include <opencv2/imgproc/imgproc.hpp>
namespace DesignNet{
/*!
 *	\brief 直方图类型数据
 *  
 *  
 */
class DESIGNNET_CORE_EXPORT HistogramData : public IData
{
	Q_OBJECT

public:
	HistogramData(QObject *parent);
	~HistogramData();
	virtual Core::Id id();
	virtual IData* clone(QObject *parent = 0);
	virtual bool copy(IData* data);
	void setHistogram(const cv::Mat &hist);
	cv::Mat histogram();
	virtual bool isValid() const;
	virtual QImage image();
private:
	cv::Mat m_hist;//!< 直方图
};

}

#endif // HISTOGRAMDATA_H
