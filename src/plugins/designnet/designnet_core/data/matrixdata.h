#ifndef MATRIXDATA_H
#define MATRIXDATA_H

#include "idata.h"
#include <opencv2/core/core.hpp>
namespace DesignNet{

class DESIGNNET_CORE_EXPORT MatrixData : public IData
{
	Q_OBJECT

public:
	MatrixData(QObject *parent = 0);
	~MatrixData();
	virtual Core::Id id();
	virtual IData* clone(QObject *parent = 0);
	virtual bool copy(IData* data);
	void setMatrix(const cv::Mat &matrix);
	cv::Mat &getMatrix();
	virtual bool isValid() const;
	virtual QImage image();
private:
	cv::Mat m_matrix;//!< MatrixÊý¾Ý
};

}

#endif // MATRIXDATA_H
