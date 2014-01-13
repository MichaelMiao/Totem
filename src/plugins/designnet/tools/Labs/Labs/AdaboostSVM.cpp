#include "stdafx.h"
#include "AdaboostSVM.h"
#include <QDir>
#include <QFileInfo>
#include <QVector>
#include <QMap>
#include <QTextStream>
#include "opencv2\highgui\highgui.hpp"
#include "FlowerFeatureExtractor.h"
#include "opencv2/ml/ml.hpp"
#include "opencv2/core/core.hpp"
#include "opencvlibsvm.h"
#include "opencv2/nonfree/ocl.hpp"
#include "opencv2/nonfree/features2d.hpp"


#define TEST_PATH		"I:/data/_test/"
#define TRAIN_PATH		"I:/data/_train/"
#define TRAIN_SIFT_PATH	"I:/data/sift/"
#define TEST_PATH_SRC	"I:/data/test/"
#define TRAIN_PATH_SRC	"I:/data/train/"

#define LABEL_FILE		"I:/data/labels.txt"
#define TYPE_NAME		"I:/data/typename.txt"
#define SVM_MODEL_FILE	"I:/data/svm.xml"
#define SVM_MODEL_FILE2	"I:/data/svm_normalize.xml"
#define SVM_BOW_FILE	"I:/data/bow.xml"
#define BOW_MAT_NAME	"bow"

AdaboostSVMProcessor::AdaboostSVMProcessor(DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/)
{
	setName(tr("AdaboostSVM"));
}

AdaboostSVMProcessor::~AdaboostSVMProcessor(void)
{

}

QString AdaboostSVMProcessor::category() const
{
	return tr("Labs");
}

bool AdaboostSVMProcessor::prepareProcess()
{
	return true;
}

void AdaboostSVMProcessor::prepareData()
{
	QString strTrain	= TRAIN_PATH_SRC;
	QString strTest		= TEST_PATH_SRC;
	QDir dir(strTrain);
	QStringList filters;

	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Dirs | QDir::NoDotAndDotDot);
	dir.setPath(strTest);
	QFileInfoList testFileList = dir.entryInfoList(filters, QDir::Dirs | QDir::NoDotAndDotDot);
	fileList << testFileList;
	QVector<QString> typeVec;
	QMap<int, int> imgLabel;	// img: id, label
	filters << "*.bmp";

	for (int i = 0; i < fileList.size(); i++)
	{
		QString fileName = fileList.at(i).fileName();
		int iLabel = typeVec.indexOf(fileName);
		bool bTest = i >= testFileList.size();

		if (iLabel == -1)
		{
			iLabel = typeVec.size();
			typeVec.push_back(fileName);
		}

		QDir dirSub(fileList.at(i).filePath());
		QFileInfoList imgList = dirSub.entryInfoList(filters, QDir::Files);
		for (int j = 0; j < imgList.size(); j++)
		{
			QString imgName = imgList.at(j).baseName();
			int id = imgName.toInt();
			if (imgLabel.contains(id))
			{
				qDebug() << "error" << endl;
			}
			else
			{
				imgLabel[id] = iLabel;
				QFile::copy(imgList.at(j).absoluteFilePath(), tr(bTest ? "I:/data/_test/%1" : "I:/data/_train/%1").arg(imgList.at(j).fileName()));
//				cv::Mat mat = cv::imread(imgList.at(j).absoluteFilePath().toLocal8Bit().data());
			}
		}
	}

	QFile fileLabel(LABEL_FILE);
	if (!fileLabel.open(QIODevice::WriteOnly))
		return ;
	QTextStream s(&fileLabel);
	for (QMap<int, int>::iterator itr = imgLabel.begin(); itr != imgLabel.end(); itr++)
	{
		s << itr.key() << "\t" << itr.value() << endl;
	}
	fileLabel.close();

	QFile fileType(TYPE_NAME);
	if (!fileType.open(QIODevice::WriteOnly))
		return ;
	QTextStream sType(&fileType);
	int i = 0;
	for (QVector<QString>::iterator itr = typeVec.begin(); itr != typeVec.end(); itr++)
	{
		sType << i++ << "\t" << *itr << endl;
	}
	fileLabel.close();
}

void normalizeFeature(cv::Mat &mat, cv::Mat &matMaxMin)
{
	if (matMaxMin.rows == 0)
	{
		matMaxMin = cv::Mat(2, mat.cols, CV_32FC1);
		for (int c = 0; c < mat.cols; c++)
		{
			float fMax = -3000, fMin = 3000;
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
					continue;
				pData[c] = (pData[c] - fMin) / (fMax - fMin);
			}
			matMaxMin.at<float>(0, c) = fMin;
			matMaxMin.at<float>(1, c) = fMax;
		}
	}
}

bool AdaboostSVMProcessor::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
//	prepareData();
	train();
//	test();
	return true;
}

bool AdaboostSVMProcessor::finishProcess()
{
	return true;
}

void AdaboostSVMProcessor::serialize(Utils::XmlSerializer& s) const
{

}

void AdaboostSVMProcessor::deserialize(Utils::XmlDeserializer& s)
{

}

void AdaboostSVMProcessor::LoadLabel(QMap<int, int> &imgLabel)
{
	QFile fileLabel(LABEL_FILE);
	if (!fileLabel.open(QIODevice::ReadOnly))
		return;
	QTextStream s(&fileLabel);
	int id, iLabel;
	while (!s.atEnd())
	{
		QString str = s.readLine();
		QTextStream sTemp(&str);
		sTemp >> id >> iLabel;
		imgLabel[id] = iLabel;
	}
	fileLabel.close();
}

void AdaboostSVMProcessor::train()
{
	QString strTrain = TRAIN_PATH;
	QDir dir(strTrain);
	QStringList filters;
	filters << "*.bmp";
	QMap<int, int> imgLabel;	// img: id, label
	LoadLabel(imgLabel);
	cv::Mat labels;
	cv::Mat trainData;
	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
	cv::SIFT sift(100);
	cv::Mat dictionary;
	int iDictionarySize = 100;
	cv::BOWKMeansTrainer bowTrainer(iDictionarySize, cv::TermCriteria(CV_TERMCRIT_ITER, 20,0.001), 3, cv::KMEANS_PP_CENTERS);
	cv::Mat siftFeatures;
	std::vector<cv::Mat> vecMat;
	for (int i = 0; i < fileList.size(); i++)
	{
		QString filePath = fileList.at(i).filePath();
		QString fileName = fileList.at(i).baseName();
		int iId = fileName.toInt();
		int iLabel = imgLabel.value(iId);
		labels.push_back(iLabel);
		emit logout(fileName);
		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
		std::vector<cv::KeyPoint> vecKeypoint;
		cv::Mat desc;
		sift.detect(mat, vecKeypoint);
		sift.compute(mat, vecKeypoint, desc);
		vecMat.push_back(mat);
		siftFeatures.push_back(desc);
 		FlowerFeatureExtractor f;
		trainData.push_back(f.extractFeature(mat));
	}
// 	cv::FileStorage fs(SVM_BOW_FILE, cv::FileStorage::WRITE);
// 	dictionary = bowTrainer.cluster(siftFeatures);
// 	fs << BOW_MAT_NAME << dictionary;
// 	fs.release();
	cv::Ptr<cv::DescriptorMatcher> matcher(new cv::FlannBasedMatcher);
	cv::Ptr<cv::FeatureDetector> detector(new cv::SiftFeatureDetector);
	cv::Ptr<cv::DescriptorExtractor> extractor(new cv::SiftDescriptorExtractor);    
	cv::BOWImgDescriptorExtractor bowDE(extractor, matcher);
	bowDE.setVocabulary(dictionary);
	for (size_t i = 0; i < vecMat.size(); i++)
	{
		std::vector<cv::KeyPoint> vecKeypoint;
		cv::Mat m = vecMat.at(i);
		cv::Mat mdesc;
		sift.detect(m, vecKeypoint);
		bowDE.compute(m, vecKeypoint, mdesc);
	}
	cv::Mat matNormalize;
	emit logout("normalizeFeature..");
	normalizeFeature(trainData, matNormalize);
	cv::FileStorage fsfesture(SVM_MODEL_FILE2, cv::FileStorage::WRITE);
	fsfesture << "minmax" << matNormalize;
	fsfesture << "test" << trainData;
	fsfesture.release();
	emit logout("train..");
	OpenCVLibSVM svm;
	svm.train(trainData, labels);
// 	cv::SVMParams param;
// 	param.kernel_type = CvSVM::RBF;
// 	param.degree = 3;
// 	param.gamma = 1.0 / 12;
// 	param.term_crit = cv::TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, FLT_EPSILON);
// 	CvSVM svm;
// 	svm.train_auto(trainData, labels, cv::Mat(), cv::Mat(), param);
// 	svm.save(SVM_MODEL_FILE);
	emit logout("finish train");
}

void AdaboostSVMProcessor::test()
{
	QString strTest = TEST_PATH;
	QDir dir(strTest);
	QStringList filters;
	filters << "*.bmp";
	QMap<int, int> imgLabel;	// img: id, label
	LoadLabel(imgLabel);
	OpenCVLibSVM svm;
	svm.loadModel();
	cv::FileStorage fsMinMax(SVM_MODEL_FILE2, cv::FileStorage::READ);
	cv::FileNode node = fsMinMax.root();
	cv::FileNodeIterator itr = node.begin();
	cv::Mat featureMinMax;
	while (itr != node.end())
	{
		std::string strName = (*itr).name();
		if (strName == "minmax")
			(*itr) >> featureMinMax;
		itr++;
	}

	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
	double fSum = 0;
	for (int i = 0; i < fileList.size(); i++)
	{
		QString filePath = fileList.at(i).filePath();
		QString fileName = fileList.at(i).baseName();
		int iId = fileName.toInt();
		int iLabel = imgLabel.value(iId);
		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
		FlowerFeatureExtractor f;
		cv::Mat feature = f.extractFeature(mat);
		for (int c = 0; c < feature.cols; c++)
		{
			float fMin = featureMinMax.at<float>(0, c);
			float fMax = featureMinMax.at<float>(1, c);
			feature.at<float>(0, c) = (feature.at<float>(0, c) - fMin) / (fMax - fMin);
		}
		int nr_class = svm.getClassCount();
		int* mm = new int[nr_class];
		svm.predict_values(feature, mm);
		//int iPredictLabel = (int)svm.test(feature);
		qDebug() << tr("%1---%2 (%3, %4, %5, %6, %7)").arg(fileName).arg(iLabel).arg(mm[0]).arg(mm[1]).arg(mm[2]).arg(mm[3]).arg(mm[4]);
		for (int i = 0; i < 5; i++)
		{
			if (mm[i] == iLabel)
			{
				fSum++;
				break;
			}
		}
		delete []mm;
// 		if (iLabel == iPredictLabel)
// 			fSum += 1;
// 		else
// 		{
// 			emit logout(tr("error: fileName: (%1, %2) -> %3  ").arg(fileName).arg(iLabel).arg(iPredictLabel));
// 		}
	}
	emit logout(tr("Accurrate %1%").arg(fSum / fileList.size() * 100));
}
