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
#include "MultiLabelKNN.h"

#define TEST_PATH		"I:/data/_test/"
#define TRAIN_PATH		"I:/data/_train/"
#define TRAIN_SIFT_PATH	"I:/data/sift/"
#define TEST_PATH_SRC	"I:/data/test/"
#define TRAIN_PATH_SRC	"I:/data/train/"

#define LABEL_FILE		"I:/data/labels.txt"
#define TYPE_NAME		"I:/data/typename.txt"
#define SVM_MODEL_FILE	"I:/data/svm.xml"
#define SVM_MODEL_FILE2	"I:/data/svm2.xml"
#define SVM_MODEL_FILE_NORM	"I:/data/svm_normalize.xml"
#define SVM_MODEL_FILE_NORM2	"I:/data/svm_normalize2.xml"
#define SVM_BOW_FILE	"I:/data/bow.xml"
#define BOW_MAT_NAME	"bow"
#define SVM_PREDICT_LABEL "I:/data/predictlabel.xml"

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

void AdaboostSVMProcessor::createBowSiftData()
{
	QDir dir(TRAIN_PATH);
	QStringList filters;
	filters << "*.bmp";
	cv::SIFT sift(100);
	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
	cv::Mat features;
	for (int i = 0; i < fileList.size(); i++)
	{
		QString filePath = fileList.at(i).filePath();
		QString fileName = fileList.at(i).baseName();
		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
		std::vector<cv::KeyPoint> keys;
		sift.detect(mat, keys);
		cv::Mat desc;
		sift.compute(mat, keys, desc);
		features.push_back(desc);
	}
	int iDictionarySize = 100;
	cv::BOWKMeansTrainer bowTrainer(iDictionarySize, cv::TermCriteria(CV_TERMCRIT_ITER, 20,0.001), 3, cv::KMEANS_PP_CENTERS);
	cv::Mat dictionary = bowTrainer.cluster(features);
	cv::FileStorage fs(SVM_BOW_FILE, cv::FileStorage::WRITE);
	fs << BOW_MAT_NAME << dictionary;
	fs.release();
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
	test();
//	createBowSiftData();
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

// void testSift
// {
// 	cv::SIFT sift;
// 	cv::Mat mat = cv::imread("I:/0271.bmp");
// 	cv::Mat mat1 = cv::imread("I:/0272.bmp");
// 	cv::Mat desc, desc1;
// 	std::vector<cv::KeyPoint> keys, keys1;
// 	sift.detect(mat, keys);
// 	sift.detect(mat1, keys1);
// 	sift.compute(mat, keys, desc);
// 	sift.compute(mat1, keys1, desc1);
// 	cv::drawKeypoints(mat, keys, mat);
// 	cv::drawKeypoints(mat1, keys1, mat1);
// 	std::vector<cv::DMatch> matches;
// 	cv::Ptr<cv::DescriptorMatcher> descriptor_matcher = cv::DescriptorMatcher::create( "BruteForce" );//创建特征匹配器  
// 	descriptor_matcher->match(desc, desc1, matches);
// 	//计算匹配结果中距离的最大和最小值  
// 	//距离是指两个特征向量间的欧式距离，表明两个特征的差异，值越小表明两个特征点越接近  
// 	double max_dist = 0;  
// 	double min_dist = 100;  
// 	for(int i=0; i<matches.size(); i++)  
// 	{  
// 		double dist = matches[i].distance;  
// 		if(dist < min_dist) min_dist = dist;  
// 		if(dist > max_dist) max_dist = dist;  
// 	}  
// 	//筛选出较好的匹配点  
// 	std::vector<cv::DMatch> goodMatches;  
// 	for(int i=0; i<matches.size(); i++)  
// 	{  
// 		if(matches[i].distance <= 0.31 * max_dist)  
// 		{  
// 			goodMatches.push_back(matches[i]);  
// 		}  
// 	}
// 	//画出匹配结果  
// 	cv::Mat img_matches;  
// 	//红色连接的是匹配的特征点对，绿色是未匹配的特征点  
// 	cv::drawMatches(mat,keys,mat1,keys1,goodMatches,img_matches,  
// 		cv::Scalar::all(-1)/*CV_RGB(255,0,0)*/,CV_RGB(0,255,0), cv::Mat(),2);  
// 	cv::imwrite("I:/match.bmp", img_matches);
// 	return;
// };

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
	
	int iDictionarySize = 500;
	cv::BOWKMeansTrainer bowTrainer(iDictionarySize, cv::TermCriteria(CV_TERMCRIT_ITER, 20,0.001), 3, cv::KMEANS_PP_CENTERS);
	for (int i = 0; i < fileList.size(); i++)
	{
		QString filePath = fileList.at(i).filePath();
		QString fileName = fileList.at(i).baseName();
		int iId = fileName.toInt();
		int iLabel = imgLabel.value(iId);
		labels.push_back(iLabel);
		emit logout(fileName);
		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
		FlowerFeatureExtractor f;
		cv::Mat feature = f.extractFeature(mat);
		trainData.push_back(feature);
	}
	trainData.convertTo(trainData, CV_32F);
	cv::Mat matNormalize;
	emit logout("normalizeFeature..");
	normalizeFeature(trainData, matNormalize);
	cv::FileStorage fsfesture(SVM_MODEL_FILE_NORM, cv::FileStorage::WRITE);
	fsfesture << "minmax" << matNormalize;
	fsfesture.release();
	emit logout("train..");
	OpenCVLibSVM svm;
	svm.train(trainData, labels, SVM_MODEL_FILE);
	return;
	int *labelVoted = new int[svm.getClassCount()];
	double *probility = new double[svm.getClassCount()];
	cv::Mat labelMat;
	cv::Mat probMat;
	for (int i = 0; i < fileList.size(); i++)
	{
		QString filePath = fileList.at(i).filePath();
		QString fileName = fileList.at(i).baseName();
		emit logout(fileName);
		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
		FlowerFeatureExtractor f;
		cv::Mat imageFeature = trainData.row(i);
		cv::Mat featureLabel(1, 6, CV_32SC1);
		cv::Mat prob(1, 6, CV_32FC1);
		svm.predict_probility(imageFeature, labelVoted, probility);
		for (int i = 0; i < 6; i++)
		{
			featureLabel.at<int>(0, i) = labelVoted[i];
			prob.at<float>(0, i) = probility[i];
		}
		probMat.push_back(prob);
		labelMat.push_back(featureLabel);
	}
	
	delete []labelVoted;
	delete []probility;

//	MultiLabelKNN knn;
//	knn.train2(labels, labelMat, svm.getClassCount());
//	knn.save("I:/data/knn.xml");
// 	cv::FileStorage fsfesture(SVM_PREDICT_LABEL, cv::FileStorage::WRITE);
// 	fsfesture << "predict" << labelMat;
// 	fsfesture.release();
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
	cv::Mat labelMat;
	cv::FileStorage fsfesture(SVM_PREDICT_LABEL, cv::FileStorage::READ);
	fsfesture["predict"] >> labelMat;
	fsfesture.release();

	QString strTest = TEST_PATH;
	QDir dir(strTest);
	QStringList filters;
	filters << "*.bmp";
	QMap<int, int> imgLabel;	// img: id, label
	LoadLabel(imgLabel);
	OpenCVLibSVM svm;
	svm.loadModel();
	cv::FileStorage fsMinMax(SVM_MODEL_FILE_NORM, cv::FileStorage::READ);
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
	MultiLabelKNN knn;
//	knn.load("I:/data/knn.xml");
	knn.LoadData();
	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
	double fSum = 0;
	double fSum5 = 0;
	for (int i = 0; i < fileList.size(); i++)
	{
		QString filePath = fileList.at(i).filePath();
		QString fileName = fileList.at(i).baseName();
		int iId = fileName.toInt();
		int iLabel = imgLabel.value(iId);
		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
		FlowerFeatureExtractor f;
		cv::Mat feature = f.extractFeature(mat);
		feature.convertTo(feature, CV_32F);
		for (int c = 0; c < feature.cols; c++)
		{
			float fMin = featureMinMax.at<float>(0, c);
			float fMax = featureMinMax.at<float>(1, c);
			feature.at<float>(0, c) = (feature.at<float>(0, c) - fMin) / (fMax - fMin);
		}
		int *labelVoted = new int[svm.getClassCount()];
		double *probility = new double[svm.getClassCount()];
		svm.predict_probility(feature, labelVoted, probility);
		cv::Mat labelF(1, 6, CV_32SC1);
		cv::Mat prob(1, 6, CV_32FC1);
		for (int i = 0; i < 6; i++)
		{
			labelF.at<int>(0, i) = labelVoted[i];
			prob.at<float>(0, i) = probility[i];
		}
		int iPredictLabel = knn.predict2(labelF, prob);
		qDebug() << iLabel << "-->" << labelVoted[0] << "---" << iPredictLabel;
//		qDebug() << iLabel << "-->" << knn.predict(labelF, prob);
		for (int i = 0; i < 5; i++)
		{
			if (labelVoted[i] == iLabel)
				fSum5 += 1;
		}
 		if (iLabel == labelVoted[0])
		{
			fSum += 1;
		}
		else
		{
			qDebug() << "==========";
			emit logout(tr("error: fileName: (%1, %2) -> %3  ").arg(fileName).arg(iLabel).arg(iPredictLabel));
		}
		delete []labelVoted;
		delete []probility;
	}
	emit logout(tr("Top1 Accurrate %1%").arg(fSum / fileList.size() * 100));
	emit logout(tr("Top5 Accurrate %1%").arg(fSum5 / fileList.size() * 100));
}