#include "StdAfx.h"
#include "MultiLabelKNN.h"
#include <QtDebug>
#include <fstream>
#define LABEL_FEATURES			"features"
#define LABEL_LABELS			"labels"
#define LABEL_CLASS_COUNT		"classcount"
#define LABEL_EH0				"EH0"
#define LABEL_EH1				"EH1"
#define LABEL_H					"H"
#define KNN_FILE				"I:/data/knn.xml"

MultiLabelKNN::MultiLabelKNN(void)
{
	m_dSmooth	= 1;
	m_iK		= 6;
}


MultiLabelKNN::~MultiLabelKNN(void)
{
}

void MultiLabelKNN::train(cv::Mat labelProbility, cv::Mat labelsExmaple, int iClassCount)
{
	if (iClassCount == 0)
		return;

	m_iClassCount			= iClassCount;
	m_matLabelExmaple		= labelsExmaple;
	m_matLabelProbilities	= labelProbility;
	m_matE_H0	= cv::Mat(m_iK + 1, iClassCount, CV_32FC1);
	m_matE_H1	= cv::Mat(m_iK + 1, iClassCount, CV_32FC1);
	m_matH		= cv::Mat(1, iClassCount, CV_32FC1);

	int m = labelProbility.rows;//!< 样本个数


	/// 求先验概率
	double *dBelongTo = new double[iClassCount];
	memset((void*)dBelongTo, 0, sizeof(double) * iClassCount);
	for (int i = 0; i < m; i++)
	{
		int *pLabel = labelsExmaple.ptr<int>(i);
		for (int c = 0; c < labelsExmaple.cols; c++)
			++dBelongTo[pLabel[c] ];
	}
	int iTemp = m_dSmooth * (m_iK + 1);

	std::vector<std::vector<int> > vecAllNearest(m);
	for (int i = 0; i < m; i++)
		vecAllNearest[i] = getNearest(m_matLabelExmaple.row(i), m_matLabelProbilities.row(i)); // 最近的几个特征的index
	
	for (size_t t = 0; t < iClassCount; t++)
	{
		m_matH.at<float>(0, t) = (m_dSmooth + dBelongTo[t]) / (m_dSmooth * 2 + m);
		std::vector<int> vecC(m_iK + 1, 0);//!< 论文里的c
		std::vector<int> vecC_(m_iK + 1, 0);//!< 论文里的c'
		for (int i = 0; i < m; i++)
		{
			std::vector<int> &vecNearest = vecAllNearest[i];
			int iCount = 0;
			for (size_t j = 0; j < vecNearest.size(); j++) // 在它的KNN中有多少个有L标签
				iCount += hasLabel(labelsExmaple.row(vecNearest[j]), t) ? 1 : 0;

			if (hasLabel(labelsExmaple.row(i), t)) // 周围有iCount个有这个标签，而自己也是这个标签
				vecC[iCount]++;
			else
				vecC_[iCount]++;
		}
		int iSumC = 0, iSumC_ = 0;
		for (size_t i = 0; i < vecC.size(); i++)
			iSumC += vecC[i];
		for (size_t i = 0; i < vecC_.size(); i++)
			iSumC_ += vecC_[i];

		/// 计算后验概率
		for (int i = 0; i <= m_iK; i++)
		{
			m_matE_H0.at<float>(i, t) = (m_dSmooth + vecC_[i]) / (iTemp + iSumC_);
			m_matE_H1.at<float>(i, t) = (m_dSmooth + vecC[i]) / (iTemp + iSumC);
		}
	}
}

void MultiLabelKNN::train2(cv::Mat labels, cv::Mat labelsExmaple, int iClassCount)
{
	std::vector<cv::Mat> features;//!< iClassCount个
	std::multimap<int, int> labelsMap;
	for (int r = 0; r < labelsExmaple.rows; r++)
		labelsMap.insert(std::make_pair(labels.at<int>(r, 0), r));
	std::ofstream ofile("i:/data/labelsoutput");

	for (int i = 0; i < iClassCount; i++)
	{
		cv::Mat mat = cv::Mat::zeros(labelsExmaple.cols, iClassCount, CV_32FC1);
		float *pData1 = mat.ptr<float>(0);
		std::pair <std::multimap<int,int>::iterator, std::multimap<int,int>::iterator> ret;
		ret = labelsMap.equal_range(i);
		ofile << i << "\r\n";
		for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
		{
			int iRow = it->second;
			for (int c = 0; c < labelsExmaple.cols; c++)
			{
				int iTemp = labelsExmaple.at<int>(iRow, c);
				ofile << iTemp << "\t";
				mat.at<float>(c, iTemp)++;
			}
			ofile << "\r\n";
		}
		ofile << "\r\n" << "\r\n";
		for (int m = 1; m < mat.rows; m++)
		{
			float *pData1 = mat.ptr<float>(m - 1);
			float *pData = mat.ptr<float>(m);
			for (int c = 0; c < iClassCount; c++)
			{
				mat.at<float>(m, c) = mat.at<float>(m, c) + mat.at<float>(m - 1, c);
			}
		}
		int iLabelCount = labelsMap.count(i);
		if (iLabelCount > 0)
		{
			for (int m = 0; m < mat.rows; m++)
			{
				float *pData = mat.ptr<float>(m);
				for (int c = 0; c < iClassCount; c++)
					mat.at<float>(m, c) = mat.at<float>(m, c) / iLabelCount;
			}
		}
		features.push_back(mat);
	}
	ofile.close();
	cv::FileStorage fs(KNN_FILE, cv::FileStorage::WRITE);

	for (size_t t = 0; t < features.size(); t++)
	{
		char ch[255];
		sprintf(ch, "feature%d", t);
		fs << ch << features.at(t);
	}
	fs.release();
}

int MultiLabelKNN::predict(cv::Mat fLabel, cv::Mat featureWeight)
{
	std::vector<int> N_t = getNearest(fLabel, featureWeight);
	double rMax = 0;
	int ret = -1;
	for (size_t t = 0; t < m_iClassCount; t++)
	{
		int iCount = 0;
		for (size_t j = 0; j < N_t.size(); j++) // 在它的KNN中有多少个有L标签
			iCount += hasLabel(fLabel, t) ? 1 : 0;
		
		double pH1E = m_matH.at<float>(0, t) * m_matE_H1.at<float>(iCount, t);
		double d = m_matH.at<float>(0, t) * m_matE_H0.at<float>(iCount, t) + pH1E;
		double r = pH1E / d;
		if (rMax < r)
		{
			rMax = r;
			ret = t;
		}
	}
	return ret;
}

static int compareDis(std::pair<int, double> p1,std::pair<int, double> p2)
{
	return p2.second > p1.second;
}

std::vector<int> MultiLabelKNN::getNearest(cv::Mat fLabel, cv::Mat featureWeight)
{
	std::vector<int> vecRes(m_iK, 0);
	std::vector<std::pair<int, double> > vecDis(m_matLabelExmaple.rows);
	int *pData = fLabel.ptr<int>(0);
	float *pProbility = featureWeight.ptr<float>(0);
	for (int i = 0; i < m_matLabelExmaple.rows; i++)
	{
		double dis = 0;
		for (int j = 0; j < fLabel.cols; j++)
		{
			for (int c = 0; c < m_matLabelExmaple.cols; c++)
			{
				if (pData[j] == m_matLabelExmaple.at<int>(i, c))
					continue;
				dis += (pProbility[c] + m_matLabelProbilities.at<float>(i, c)) * abs(c - j);
			}
		}
		
		vecDis[i] = std::make_pair(i, dis);
	}
	sort(vecDis.begin(), vecDis.end(), compareDis);
	for (int i = 0; i < m_iK; i++)
		vecRes[i] = vecDis[i].first;
	return vecRes;
}

bool MultiLabelKNN::hasLabel(cv::Mat &labelFeature, int iLabel)
{
	for (int i = 0; i < labelFeature.cols; i++)
	{
		if (labelFeature.at<int>(0, i) == iLabel)
			return true;
	}
	return false;
}

void MultiLabelKNN::save(char *filePath)
{
	cv::FileStorage fs(filePath, cv::FileStorage::WRITE);
	fs << LABEL_FEATURES << m_matLabelProbilities;
	fs << LABEL_LABELS << m_matLabelExmaple;
	fs << LABEL_CLASS_COUNT << m_iClassCount;
	fs << LABEL_EH0 << m_matE_H0;
	fs << LABEL_EH1 << m_matE_H1;
	fs << LABEL_H << m_matH;
	fs.release();
}

void MultiLabelKNN::load(char *filePath)
{
	cv::FileStorage fs(filePath, cv::FileStorage::READ);
	fs[LABEL_FEATURES] >> m_matLabelProbilities;
	fs[LABEL_LABELS] >> m_matLabelExmaple;
	fs[LABEL_CLASS_COUNT] >> m_iClassCount;
	fs[LABEL_EH0] >> m_matE_H0;
	fs[LABEL_EH1] >> m_matE_H1;
	fs[LABEL_H] >> m_matH;
	fs.release();
}

int MultiLabelKNN::predict2(cv::Mat fLabel, cv::Mat labelWeight)
{
	int iLabel = 0;
	double dBelieve = 0;
	for (size_t t = 0; t < m_vecFeature.size(); t++)
	{
		double db = 0;
		cv::Mat m = m_vecFeature.at(t);
		for (int i = 0; i < fLabel.cols; i++)
		{
			int label = fLabel.at<int>(0, i);
			for (int r = 0; r < m.rows; r++)
				db += m.at<float>(r, label) * labelWeight.at<float>(0, i);
		}
		if (dBelieve < db)
		{
			iLabel = t;
			dBelieve = db;
		}
	}
	qDebug() << "believe:" << dBelieve;
	return iLabel;
}

void MultiLabelKNN::LoadData()
{
	cv::FileStorage fs(KNN_FILE, cv::FileStorage::READ);
	cv::FileNode node = fs.root();
	cv::FileNodeIterator itr = node.begin();
	while (itr != node.end())
	{
		cv::Mat feature;
		(*itr) >> feature;
		m_vecFeature.push_back(feature);
		itr++;
	}
	fs.release();
}
