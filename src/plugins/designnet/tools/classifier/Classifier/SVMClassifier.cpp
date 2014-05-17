#include "stdafx.h"
#include "SVMClassifier.h"
#include "algrithom/opencvlibsvm.h"
#include "SVMFrontWidget.h"


#define SVM_MODEL_FILE_NORM	"G:/data/68Data/svm_normalize.xml"
#define SVM_MODEL_FILE		"G:/data/68Data/svm.xml"

enum PortIndex
{
	PortIndex_In_ExampleMat,
	PortIndex_In_PredictFeature,
	PortIndex_In_LabelMat,
	PortIndex_In_PredictedLabel,
};

static PortData s_ports[] =
{
	{ Port::IN_PORT,	DATATYPE_MATRIX,		"Example Datas" },
	{ Port::IN_PORT,	DATATYPE_MATRIX,		"Predict Feature" },
	{ Port::IN_PORT,	DATATYPE_MATRIX,		"Label Datas" },
	{ Port::OUT_PORT,	DATATYPE_MATRIX,		"Predicted Label" },
};

SVMClassifer::SVMClassifer(DesignNet::DesignNetSpace *space, QObject *parent)
{
	m_propBoolTrain		= new BoolProperty("Train", "Train", this);
	m_propDoubleRange	= new DoubleRangeProperty("Class Count", "Class Count", this);
	m_pWidget			= new SVMFrontWidget(this);
	m_propDoubleRange->setRange(1, 10);
	m_propDoubleRange->setDecimals(0);
	m_propDoubleRange->setValue(5);
	m_propBoolTrain->setValue(true);
	setName(tr("SVM"));
	addProperty(m_propBoolTrain);
	addProperty(m_propDoubleRange);
	for (int i = 0; i < _countof(s_ports); i++)
		addPort(s_ports[i].ePortType, s_ports[i].eDataType, s_ports[i].strName);
}

SVMClassifer::~SVMClassifer()
{

}

QString SVMClassifer::title() const
{
	return tr("SVM");
}

QString SVMClassifer::category() const
{
	return tr("Classifier");
}

static void normalizeFeature(cv::Mat &mat, cv::Mat &matMaxMin)
{
	if (matMaxMin.rows == 0)
	{
		matMaxMin = cv::Mat(2, mat.cols, CV_32FC1);
		for (int c = 0; c < mat.cols; c++)
		{
			float fMax = -3000000, fMin = 3000000;
			for (int r = 0; r < mat.rows; r++)
			{
				float* pData = mat.ptr<float>(r);
				fMax = qMax(pData[c], fMax);
				fMin = qMin(pData[c], fMin);
			}
			for (int r = 0; r < mat.rows; r++)
			{
				float* pData = mat.ptr<float>(r);
				if (qFuzzyCompare(fMax - fMin, 0))
				{
					pData[c] = 1;
					continue;
				}
				pData[c] = (pData[c] - fMin) / (fMax - fMin);
			}
			matMaxMin.at<float>(0, c) = fMin;
			matMaxMin.at<float>(1, c) = fMax;
		}
	}
}

bool SVMClassifer::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	bool bTrain = m_propBoolTrain->value();
	if (bTrain) // ÑµÁ·½×¶Î
	{
		cv::Mat matExample	= getPortData<cv::Mat>(s_ports[PortIndex_In_ExampleMat]);
		cv::Mat matLabel	= getPortData<cv::Mat>(s_ports[PortIndex_In_LabelMat]);
		cv::Mat matNormalize;
		normalizeFeature(matExample, matNormalize);
		cv::FileStorage fsfesture(SVM_MODEL_FILE_NORM, cv::FileStorage::WRITE);
		fsfesture << "minmax" << matNormalize;
		fsfesture.release();

		OpenCVLibSVM svm;
		svm.train(matExample, matLabel, SVM_MODEL_FILE);
	}
	else
	{
		cv::Mat mFeature = getPortData<cv::Mat>(s_ports[PortIndex_In_PredictFeature]);
		for (int c = 0; c < mFeature.cols; c++)
		{
			float fMin = m_matNormalize.at<float>(0, c);
			float fMax = m_matNormalize.at<float>(1, c);
			if (qAbs(fMax - fMin) < 0.000001)
				mFeature.at<float>(0, c) = 1;
			else
				mFeature.at<float>(0, c) = (mFeature.at<float>(0, c) - fMin) / (fMax - fMin);
		}

		int *labelVoted = new int[m_svm.getClassCount()];
		double *probility = new double[m_svm.getClassCount()];
		m_svm.predict_probility(mFeature, labelVoted, probility);
		int iCount = (int)m_propDoubleRange->value();
		cv::Mat fLabel(1, iCount, CV_32SC1);
		for (int i = 0; i < iCount; i++)
			fLabel.at<int>(0, i) = labelVoted[i];
		pushData(qVariantFromValue(fLabel), DATATYPE_MATRIX, s_ports[PortIndex_In_PredictedLabel].strName);
		delete []labelVoted;
		delete []probility;
		emit classifyFinished(fLabel);
	}
	notifyProcess();
	return true;
}

bool SVMClassifer::prepareProcess()
{
	bool bTrain = m_propBoolTrain->value();
	if (!bTrain) // ²âÊÔ½×¶Î
	{
		cv::FileStorage fsMinMax(SVM_MODEL_FILE_NORM, cv::FileStorage::READ);
		cv::FileNode node = fsMinMax.root();
		cv::FileNodeIterator itr = node.begin();
		while (itr != node.end())
		{
			std::string strName = (*itr).name();
			if (strName == "minmax")
				(*itr) >> m_matNormalize;
			itr++;
		}
		m_svm.loadModel(SVM_MODEL_FILE);
	}
	return true;
}

ProcessorFrontWidget* SVMClassifer::processorFrontWidget()
{
	return m_pWidget;
}
