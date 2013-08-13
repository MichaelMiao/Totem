#include "designnetconstants.h"
#include "matrixdata.h"

namespace DesignNet{

MatrixData::MatrixData(QObject *parent)
	: IData(parent)
{
	m_image.load(QLatin1String(Constants::DATA_IMAGE_MATRIX));
}

MatrixData::~MatrixData()
{

}

Core::Id MatrixData::id()
{
	return Constants::DATA_TYPE_MATRIX;
}

IData* MatrixData::clone( QObject *parent /*= 0*/ )
{
	MatrixData *data = new MatrixData(parent);
	data->copy(this);
	return data;
}

bool MatrixData::copy( IData* data )
{
	if(data->id() != this->id())
		return false;
	MatrixData *matrix = qobject_cast<MatrixData*>(data);
	matrix->m_matrix.copyTo(m_matrix);
	return IData::copy(data);
}

void MatrixData::setMatrix(const cv::Mat &matrix )
{
	matrix.copyTo(m_matrix);
}

cv::Mat & MatrixData::getMatrix()
{
	return m_matrix;
}

bool MatrixData::isValid() const
{
	if (m_matrix.empty())
	{
		return false;
	}
	return true;
}

QImage MatrixData::image()
{
	return m_image;
}

}
