#include "stdafx.h"
#include "OpenCVKNN.h"

OpenCVKNN::OpenCVKNN()
{

}

OpenCVKNN::~OpenCVKNN()
{

}

void OpenCVKNN::train(cv::Mat matExamples, cv::Mat matLabel, int k)
{
	m_knn.train(matExamples, matLabel, cv::Mat(), false, k);
	m_k = k;
}

int OpenCVKNN::predict(cv::Mat &matExample, cv::Mat &neighbor)
{
	return m_knn.find_nearest(matExample, /*m_k*/49, cv::Mat(), neighbor, cv::Mat());
}
