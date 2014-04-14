#include "opencvlibsvm.h"
#include <QDebug>
#define Malloc(type, n) (type *)malloc((n)*sizeof(type))
struct LabelVote
{
	int iIndex;
	int iValue;
};


OpenCVLibSVM::OpenCVLibSVM() : m_pModel(0)
{
	// default values
	m_param.svm_type = C_SVC;
	m_param.kernel_type = RBF;
	m_param.degree = 3;
	m_param.gamma = 0.1;	// 1/num_features
	m_param.coef0 = 0;
	m_param.nu = 0.5;
	m_param.cache_size = 100;
	m_param.C = 18.1;
	m_param.eps = 1e-3;
	m_param.p = 0.1;
	m_param.shrinking = 1;
	m_param.probability = 1;
	m_param.nr_weight = 0;
	m_param.weight_label = NULL;
	m_param.weight = NULL;
	m_pModel = NULL;
}

OpenCVLibSVM::~OpenCVLibSVM()
{
	svm_destroy_param(&m_param);
	svm_free_and_destroy_model(&m_pModel);
}

void OpenCVLibSVM::train(cv::Mat &mat, cv::Mat &labelMat, char* strFileName)
{
	m_prob.l = mat.rows;						//!< 样本个数
	m_prob.x = Malloc(struct svm_node*, m_prob.l);
	m_prob.y = Malloc(double, labelMat.rows);	//!< 样本标签
	
	for (int r = 0; r < labelMat.rows; r++)
		m_prob.y[r] = labelMat.at<int>(r, 0);


	int iSize = 0;
	for (int r = 0; r < mat.rows; r++)
	{
		for (int c = 0; c < mat.cols; c++)
		{
			if (mat.at<float>(r, c) != 0)
				++iSize;
		}
		++iSize;
	}
	struct svm_node* x_memory = NULL;			//!< 所有Node
	x_memory = Malloc(struct svm_node, iSize);
	int iPos = 0;
	for (int r = 0; r < mat.rows; r++)
	{
		m_prob.x[r] = x_memory + iPos;
		for (int c = 0; c < mat.cols; c++)
		{
			struct svm_node *pNode = x_memory + iPos;
			if (mat.at<float>(r, c) != 0)
			{
				pNode->index = c + 1;	//!< 从1开始
				pNode->value = mat.at<float>(r, c);
				++iPos;
			}
		}
		x_memory[iPos].index = -1;
		x_memory[iPos].value = 0;
		++iPos;
	}
	m_param.gamma = 0.1;
	
	const char *error_msg;
	error_msg = svm_check_parameter(&m_prob, &m_param);
	m_pModel = svm_train(&m_prob, &m_param);
//	crossValidation();
	svm_save_model(strFileName, m_pModel);
}

double OpenCVLibSVM::crossValidation()
{
	float gamma_step = 0.1;
	float c_step = 0.1;
	float g_min = 0.1, g_max = 2;
	float c_min = 10, c_max = 20;
	float g = g_min, c = c_min;
	float f = 0, c_best = c, g_best = g_min;
	while (g < g_max)
	{
		c = c_min;
		while (c < c_max)
		{
//  			m_param.C = c;
//  			m_param.gamma = g;
			int total_correct = 0;
			double total_error = 0;
			double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
			double *target = Malloc(double, m_prob.l);

			svm_cross_validation(&m_prob, &m_param, 4, target);
			for(int i = 0; i < m_prob.l; i++)
			{
				if(target[i] == m_prob.y[i])
					++total_correct;
			}
			float corr = 100.0 * total_correct/m_prob.l;
			qDebug() << QString("Cross Validation Accuracy = %1%\n").arg(100.0 * total_correct/m_prob.l);
			if (corr > f)
			{
				c_best = c;
				g_best = g;
				f = corr;
			}
			
			free(target);
			c += c_step;
		}
		g += gamma_step;
	}
	qDebug() << QString("Cross Validation Accuracy = %1%----c =  %2-- g = %3\n").arg(f).arg(c_best).arg(g_best);
	m_param.C = c_best;
	m_param.gamma = g_best;
	m_pModel->param = m_param;
	return f;
}

void OpenCVLibSVM::loadModel(char * strFileName)
{
	m_pModel = svm_load_model(strFileName);
}

int comp(const void *a, const void *b)
{
	LabelVote* pA = (LabelVote*)a;
	LabelVote* pB = (LabelVote*)b;
	return pB->iValue > pA->iValue ? 1 : -1;
}

double OpenCVLibSVM::test(cv::Mat &mat)
{
	int iSize = 0;
	for (int c = 0; c < mat.cols; c++)
	{
		if (mat.at<float>(0, c) != 0)
			++iSize;
	}
	struct svm_node* x_memory = NULL;			//!< 所有Node
	x_memory = Malloc(struct svm_node, iSize + 1);
	int iPos = 0;
	for (int c = 0; c < mat.cols; c++)
	{
		float f = mat.at<float>(0, c);
		if (mat.at<float>(0, c) != 0)
		{
			x_memory[iPos].index = c + 1;
			x_memory[iPos++].value = mat.at<float>(0, c);
		}
	}
	x_memory[iPos].index = -1;
	x_memory[iPos].value = 0;
	float f = svm_predict(m_pModel, x_memory);
	double *probability = new double[m_pModel->nr_class];
	int res = svm_check_probability_model(m_pModel);

	double r = svm_predict_probability(m_pModel, x_memory, probability);
	free(x_memory);
	
	std::qsort(probability, m_pModel->nr_class, sizeof(double), comp);
	int iTemp = 0;
	double resMax = 0;
	for (int i = 0; i < m_pModel->nr_class; i++)
	{
		if (resMax < probability[i])
		{
			resMax = probability[i];
			iTemp = i;
		}
	}
	int iLabel = m_pModel->label[iTemp];
	if (f != iLabel)
	{
		int m = 0;
	}
	return f;
}

double* OpenCVLibSVM::predict(cv::Mat &mat)
{
	int iSize = 0;
	for (int c = 0; c < mat.cols; c++)
	{
		if (mat.at<float>(0, c) != 0)
			++iSize;
	}
	struct svm_node* x_memory = NULL;			//!< 所有Node
	x_memory = Malloc(struct svm_node, iSize + 1);
	int iPos = 0;
	for (int c = 0; c < mat.cols; c++)
	{
		float f = mat.at<float>(0, c);
		if (mat.at<float>(0, c) != 0)
		{
			x_memory[iPos].index = c + 1;
			x_memory[iPos++].value = mat.at<float>(0, c);
		}
	}
	x_memory[iPos].index = -1;
	x_memory[iPos].value = 0;
	double *probability = new double[m_pModel->nr_class];
	int res = svm_check_probability_model(m_pModel);

	svm_predict_probability(m_pModel, x_memory, probability);
	free(x_memory);

	std::qsort(probability, m_pModel->nr_class, sizeof(double), comp);
	int iTemp = 0;
	double resMax = 0;
	for (int i = 0; i < m_pModel->nr_class; i++)
	{
		if (resMax < probability[i])
		{
			resMax = probability[i];
			iTemp = i;
		}
	}
	int iLabel = m_pModel->label[iTemp];
	return probability;
}

int OpenCVLibSVM::getClassCount()
{
	return m_pModel->nr_class;
}

void OpenCVLibSVM::predict_values(cv::Mat &mat, int *labelVoted)
{
	int iSize = 0;
	for (int c = 0; c < mat.cols; c++)
	{
		if (mat.at<float>(0, c) != 0)
			++iSize;
	}
	struct svm_node* x_memory = NULL;			//!< 所有Node
	x_memory = Malloc(struct svm_node, iSize + 1);
	int iPos = 0;
	for (int c = 0; c < mat.cols; c++)
	{
		float f = mat.at<float>(0, c);
		if (mat.at<float>(0, c) != 0)
		{
			x_memory[iPos].index = c + 1;
			x_memory[iPos++].value = mat.at<float>(0, c);
		}
	}
	x_memory[iPos].index = -1;
	x_memory[iPos].value = 0;
	memset((void*)labelVoted, 0, sizeof(int) * m_pModel->nr_class);
	int res = svm_check_probability_model(m_pModel);
	double *dec_values = new double[m_pModel->nr_class * (m_pModel->nr_class - 1) / 2];
	double m = svm_predict_values(m_pModel, x_memory, dec_values, labelVoted);
	delete []dec_values;
	LabelVote *plv = new LabelVote[m_pModel->nr_class];
	for (int i = 0; i < m_pModel->nr_class; i++)
	{
		plv[i].iIndex = i;
		plv[i].iValue = labelVoted[i];
	}
	qsort(plv, m_pModel->nr_class, sizeof(LabelVote), comp);

	for (int i = 0; i < m_pModel->nr_class; i++)
		labelVoted[i] = m_pModel->label[plv[i].iIndex];
	free(x_memory);
}

void OpenCVLibSVM::predict_probility(cv::Mat &mat, int *labelVoted, double* probility)
{
	int iSize = 0;
	for (int c = 0; c < mat.cols; c++)
	{
		if (mat.at<float>(0, c) != 0)
			++iSize;
	}
	struct svm_node* x_memory = NULL;			//!< 所有Node
	x_memory = Malloc(struct svm_node, iSize + 1);
	int iPos = 0;
	for (int c = 0; c < mat.cols; c++)
	{
		float f = mat.at<float>(0, c);
		if (mat.at<float>(0, c) != 0)
		{
			x_memory[iPos].index = c + 1;
			x_memory[iPos++].value = mat.at<float>(0, c);
		}
	}
	x_memory[iPos].index = -1;
	x_memory[iPos].value = 0;
	memset((void*)labelVoted, 0, sizeof(int) * m_pModel->nr_class);
	int res = svm_check_probability_model(m_pModel);
	double *dec_values = new double[m_pModel->nr_class * (m_pModel->nr_class - 1) / 2];
	double m = svm_predict_values(m_pModel, x_memory, dec_values, labelVoted);
	delete []dec_values;
	LabelVote *plv = new LabelVote[m_pModel->nr_class];
	for (int i = 0; i < m_pModel->nr_class; i++)
	{
		plv[i].iIndex = i;
		plv[i].iValue = labelVoted[i];
	}
	qsort(plv, m_pModel->nr_class, sizeof(LabelVote), comp);

	int iCount = m_pModel->nr_class * (m_pModel->nr_class - 1) / 2;
	
	for (int i = 0; i < m_pModel->nr_class; i++)
	{
		probility[i] = plv[i].iValue * 1.0 / iCount;
		labelVoted[i] = m_pModel->label[plv[i].iIndex];
	}

	free(x_memory);
	delete []plv;
}
