#pragma once
#include "opencv2\core\core.hpp"
/*!
	参考M.-L. Zhang, Z.-H. Zhou / Pattern Recognition 40 (2007) 2038 – 2048的MLKNN算法
*/

class MultiLabelKNN
{
public:
	MultiLabelKNN(void);
	~MultiLabelKNN(void);

	void train2(cv::Mat labels, cv::Mat labelsExmaple, int iClassCount);

	void train(cv::Mat labelProbility, cv::Mat labelsExmaple, int iClassCount);//!< 每行是一个样本
	
	void save(char *filePath);
	void load(char *filePath);

	int predict(cv::Mat fLabel, cv::Mat featureWeight);
	int predict2(cv::Mat fLabel, cv::Mat labelWeight);
	void LoadData();

protected:
	
	std::vector<int> getNearest(cv::Mat t, cv::Mat featureWeight);//!< 得到最相近的一些点
	bool hasLabel(cv::Mat &labelFeature, int iLabel);

private:

	double	m_dSmooth;		//!< 平滑变量
	int		m_iK;			//!< k近邻
	cv::Mat m_matLabelProbilities;	//!< 所有的样本
	cv::Mat m_matLabelExmaple;//!< 样本标签
	int		m_iClassCount;	//!< 样本种类
	cv::Mat m_matH;			//!< 先验概率
	cv::Mat m_matE_H0;		//!< 后验概率P(E|H0)
	cv::Mat m_matE_H1;		//!< 后验概率P(E|H1)

	std::vector<cv::Mat> m_vecFeature;
};

