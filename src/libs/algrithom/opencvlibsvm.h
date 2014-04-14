#pragma once
#include "libsvm/svm.h"
#include "opencv2/core/core.hpp"
#include "algrithom_global.h"


class ALGRITHOM_EXPORT OpenCVLibSVM
{
public:

	OpenCVLibSVM();
	~OpenCVLibSVM();

	void loadModel(char * strFileName = "I:/data/svm.xml");
	void train(cv::Mat &mat, cv::Mat &labelMat, char* strFileName = "I:/data/svm.xml"); //!< 每行是一个样本
	double crossValidation();
	double test(cv::Mat &mat);
	double* predict(cv::Mat &mat);
	void predict_values(cv::Mat &mat, int *labelVoted);
	void predict_probility(cv::Mat &mat, int *labelVoted, double* probility);
	int getClassCount();


private:

	svm_model*		m_pModel;
	svm_problem		m_prob;
	svm_parameter	m_param;
};
