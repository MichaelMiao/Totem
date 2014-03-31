#pragma once
#include "opencv2/ml/ml.hpp"
#include "opencv2\core\core.hpp"


class OpenCVKNN
{
public:

	OpenCVKNN();
	~OpenCVKNN();

	void	train(cv::Mat matExamples, cv::Mat matLabel, int k);
	int		predict(cv::Mat &matExample, cv::Mat &neighbor);

private:

	CvKNearest	m_knn;
	int			m_k;
};
