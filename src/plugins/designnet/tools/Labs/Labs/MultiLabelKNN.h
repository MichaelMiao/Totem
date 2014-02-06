#pragma once
#include "opencv2\core\core.hpp"
/*!
	�ο�M.-L. Zhang, Z.-H. Zhou / Pattern Recognition 40 (2007) 2038 �C 2048��MLKNN�㷨
*/

class MultiLabelKNN
{
public:
	MultiLabelKNN(void);
	~MultiLabelKNN(void);

	void train2(cv::Mat labels, cv::Mat labelsExmaple, int iClassCount);

	void train(cv::Mat labelProbility, cv::Mat labelsExmaple, int iClassCount);//!< ÿ����һ������
	
	void save(char *filePath);
	void load(char *filePath);

	int predict(cv::Mat fLabel, cv::Mat featureWeight);
	int predict2(cv::Mat fLabel, cv::Mat labelWeight);
	void LoadData();

protected:
	
	std::vector<int> getNearest(cv::Mat t, cv::Mat featureWeight);//!< �õ��������һЩ��
	bool hasLabel(cv::Mat &labelFeature, int iLabel);

private:

	double	m_dSmooth;		//!< ƽ������
	int		m_iK;			//!< k����
	cv::Mat m_matLabelProbilities;	//!< ���е�����
	cv::Mat m_matLabelExmaple;//!< ������ǩ
	int		m_iClassCount;	//!< ��������
	cv::Mat m_matH;			//!< �������
	cv::Mat m_matE_H0;		//!< �������P(E|H0)
	cv::Mat m_matE_H1;		//!< �������P(E|H1)

	std::vector<cv::Mat> m_vecFeature;
};

