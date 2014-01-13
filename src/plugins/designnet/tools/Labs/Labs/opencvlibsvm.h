#pragma once
#include "libsvm/svm.h"
#include "opencv2/core/core.hpp"

class OpenCVLibSVM
{
public:

	OpenCVLibSVM();
	~OpenCVLibSVM();

	void loadModel();
	void train(cv::Mat &mat, cv::Mat &labelMat); //!< 每行是一个样本
	void crossValidation();
	double test(cv::Mat &mat);
	double* predict(cv::Mat &mat);
	void predict_values(cv::Mat &mat, int *labelVoted);
	int getClassCount();


private:

	svm_model*		m_pModel;
	svm_problem		m_prob;
	svm_parameter	m_param;
};