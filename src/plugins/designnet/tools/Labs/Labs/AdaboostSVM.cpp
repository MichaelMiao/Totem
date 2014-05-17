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
#include "OpenCVKNN.h"

// #define TEST_PATH		"I:/data/_test/"
// #define TRAIN_PATH		"I:/data/_train/"
// #define TRAIN_SIFT_PATH	"I:/data/sift/"
// #define TEST_PATH_SRC	"I:/data/test/"
// #define TRAIN_PATH_SRC	"I:/data/train/"
// 
// #define LABEL_FILE		"I:/data/labels.label"
// #define TYPE_NAME		"I:/data/typename.txt"
// #define SVM_MODEL_FILE	"I:/data/svm.xml"
// #define SVM_MODEL_FILE2	"I:/data/svm2.xml"
// #define SVM_MODEL_FILE_NORM	"I:/data/svm_normalize.xml"
// #define SVM_MODEL_FILE_NORM2	"I:/data/svm_normalize2.xml"
// #define SVM_BOW_FILE	"I:/data/bow.xml"
// #define BOW_MAT_NAME	"bow"
// #define SVM_PREDICT_LABEL "I:/data/predictlabel.xml"

//#define USE_KNN


#define TEST_PATH		"G:/data/68Data/test"
#define TRAIN_PATH		"G:/data/68Data/train"
#define TRAIN_SIFT_PATH	"I:/data/sift/"
#define TEST_PATH_SRC	"G:/data/68Data/test/"
#define TRAIN_PATH_SRC	"G:/data/68Data/train/"

#define LABEL_FILE		"G:/data/68Data/label_map.txt"
#define TYPE_NAME		"I:/data/typename.txt"
#define SVM_MODEL_FILE	"G:/data/68Data/svm.xml"
#define SVM_MODEL_FILE2	"G:/data/68Data/svm2.xml"
#define SVM_MODEL_FILE_NORM	"G:/data/68Data/svm_normalize.xml"
#define SVM_MODEL_FILE_NORM2	"G:/data/68Data/svm_normalize2.xml"
#define SVM_BOW_FILE	"G:/data/68Data/bow.xml"
#define BOW_MAT_NAME	"bow"
#define SVM_PREDICT_LABEL "G:/data/68Data/predictlabel.xml"


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
 	return true;
	QStringList ls;
	QStringList folderList;
	folderList << QString::fromLocal8Bit("G:/data/68/");
	QStringList classList;
	QMultiMap<int, int> labelMap;
	int iIndex = 0;
	while (!folderList.empty())
	{
		QString folder = folderList.front();
		folderList.pop_front();
		classList << folder;

		QDir dirSub(folder);
		QFileInfoList imgList = dirSub.entryInfoList(ls, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
		QFileInfoList::iterator itr = imgList.begin();
		for(; itr != imgList.end(); itr++)
		{
			if (itr->isDir())
			{
				folderList.push_back(itr->absoluteFilePath());
			}
			else
			{
				QImage img;
				img.load((*itr).absoluteFilePath());
				img = img.scaled(320, 240, Qt::KeepAspectRatioByExpanding);
				QString s = (*itr).fileName();
				QString path = QString::fromLocal8Bit("G:/data/68Data/所有数据/");
				QString f = path + tr("%1.bmp").arg(iIndex);
				img.save(f);
				labelMap.insert(classList.size() - 1, iIndex);
				iIndex++;
			}
		}
	}
	classList.pop_front();
	QFile file_labelName("G:/data/68Data/label_name.txt");
	QFile file_labelMap("G:/data/68Data/label_map.txt");
	QFile file_labelCount("G:/data/68Data/label_mapCount.txt");
	file_labelName.open(QFile::WriteOnly);
	QStringList::iterator itr = classList.begin();
	QTextStream fs(&file_labelName);
	int iCount = 1;
	for (; itr != classList.end(); itr++)
	{
		fs << iCount++ << "   " << *itr << endl;
	}
	file_labelName.close();
	file_labelCount.open(QFile::WriteOnly);
	QTextStream fc(&file_labelCount);

	file_labelMap.open(QFile::WriteOnly);
	{
		QList<int> labels = labelMap.uniqueKeys();
		QTextStream fs(&file_labelMap);
		for (int i = 0; i < labels.size(); i++)
		{
			QList<int> v = labelMap.values(labels.at(i));
			fc << labels.at(i) << "  " << v.size() << endl;
			for (int j = 0; j < v.size(); j++)
			{
				fs << labels.at(i) << "   " << v[j] << endl;
			}
			
		}
	}
	file_labelMap.close();
	file_labelCount.close();
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
		sTemp >> iLabel >> id;
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
	m_imgLabel = imgLabel;
	cv::Mat &labels = m_labelMat;
	cv::Mat &trainData = m_train;
	cv::Mat &colorTrain = m_colorTrain;
	cv::Mat &shapeTrain = m_shapeTrain;
	cv::Mat &textureTrain = m_textureTrain;
	cv::Mat featureKMean;
	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
	
	QMultiMap<int, QString> categoryLabels;
	emit logout("start extract");
	for (int i = 0; i < fileList.size(); i++)
	{
		QString filePath = fileList.at(i).filePath();
		QString fileName = fileList.at(i).baseName();
		m_vecFiles.push_back(filePath);
		int iId = fileName.toInt();
		int iLabel = imgLabel.value(iId);
		labels.push_back(iLabel);
		categoryLabels.insert(iLabel, filePath);
		emit logout(fileName);
		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
		FlowerFeatureExtractor f(mat);
		cv::Mat clrF = f.extractColor();
		cv::Mat shapeF = f.extractShape2();
		cv::Mat glcmF = f.extractGLCM();
		cv::Mat F;
		F.push_back(clrF.reshape(1, clrF.cols));
		F.push_back(shapeF.reshape(1, shapeF.cols));
		F.push_back(glcmF.reshape(1, glcmF.cols));
		F = F.reshape(1, 1);
		trainData.push_back(F);
		colorTrain.push_back(clrF);
		shapeTrain.push_back(shapeF);
		textureTrain.push_back(glcmF);
	}
	emit logout("finish extract");
// 	trainData.convertTo(trainData, CV_32F);
  	cv::Mat matNormalize;
 	emit logout("normalizeFeature..");
 	normalizeFeature(m_textureTrain, matNormalize);
	cv::FileStorage fsfesture(SVM_MODEL_FILE_NORM, cv::FileStorage::WRITE);
	fsfesture << "minmax" << matNormalize;
	fsfesture.release();
// 	emit logout("train..");
// 
 	normalizeFeature(colorTrain, matNormalize);
 	normalizeFeature(shapeTrain, matNormalize);
 	normalizeFeature(textureTrain, matNormalize);
// 	
// 
#ifndef USE_KNN
	OpenCVLibSVM svm;
	svm.train(m_textureTrain, labels, SVM_MODEL_FILE);
	svm.crossValidation();
#endif
	return;
// 	int *labelVoted = new int[svm.getClassCount()];
// 	double *probility = new double[svm.getClassCount()];
// 	cv::Mat labelMat;
// 	cv::Mat probMat;
// 	for (int i = 0; i < fileList.size(); i++)
// 	{
// 		QString filePath = fileList.at(i).filePath();
// 		QString fileName = fileList.at(i).baseName();
// 		emit logout(fileName);
// 		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
// 		FlowerFeatureExtractor f(mat);
// 		cv::Mat imageFeature = trainData.row(i);
// 		cv::Mat featureLabel(1, 5, CV_32SC1);
// 		cv::Mat prob(1, 5, CV_32FC1);
// 		svm.predict_probility(imageFeature, labelVoted, probility);
// 		for (int i = 0; i < 5; i++)
// 		{
// 			featureLabel.at<int>(0, i) = labelVoted[i];
// 			prob.at<float>(0, i) = probility[i];
// 		}
// 		probMat.push_back(prob);
// 		labelMat.push_back(featureLabel);
// 	}
// 	
// 	QMap<int, QSet<int> > labelAvaliableMap;
// 	for (int r = 0; r < labelMat.rows; r++)
// 	{
// 		int *pData = labelMat.ptr<int>(r);
// 		int iLabel = labels.at<int>(r, 0);
// 		QSet<int> &d = labelAvaliableMap[iLabel];
// 		for (int c = 0; c < labelMat.cols; c++)
// 			d << pData[c];
// 	}
// 	QList<int> labelAva = labelAvaliableMap.keys();
// 	for (QList<int>::iterator itr = labelAva.begin(); itr != labelAva.end(); itr++)
// 	{
// 		OpenCVLibSVM svmTop5;
// 		cv::Mat trainData_top5;
// 		cv::Mat trainData_top5_c;
// 		cv::Mat trainData_top5_s;
// 		cv::Mat trainData_top5_t;
// 		cv::Mat label_top5;
// 		QSet<int> labelTrain = labelAvaliableMap.value(*itr);
// 		for (int r = 0; r < labels.rows; r++)
// 		{
// 			int iLabel = labels.at<int>(r, 0);
// 			if (labelTrain.contains(iLabel))
// 			{
// 				trainData_top5_c.push_back(colorTrain.row(r));
// 				trainData_top5_s.push_back(shapeTrain.row(r));
// 				trainData_top5_t.push_back(textureTrain.row(r));
// 				trainData_top5.push_back(trainData.row(r));
// 				label_top5.push_back(iLabel);
// 			}
// 		}
// 		svmTop5.train(trainData_top5_s, label_top5,
// 			QString("I:/data/%1_top5_s.xml").arg(*itr).toLocal8Bit().data());
// //		double s = svmTop5.crossValidation();
// 		svmTop5.train(trainData_top5_t, label_top5,
// 			QString("I:/data/%1_top5_t.xml").arg(*itr).toLocal8Bit().data());
// //		double t = svmTop5.crossValidation();
// 		
// 		svmTop5.train(trainData_top5_c, label_top5,
// 			QString("I:/data/%1_top5_c.xml").arg(*itr).toLocal8Bit().data());
// //		double c = svmTop5.crossValidation();
// 
// 		svmTop5.train(trainData_top5, label_top5,
// 			QString("I:/data/%1_top5.xml").arg(*itr).toLocal8Bit().data());
// //		double com = svmTop5.crossValidation();
// //		qDebug() << QString("lable =%1 c=%2 s=%3 t=%4 com=%5").arg(*itr).arg(c).arg(s).arg(t).arg(com);
// 	}
// 
// 	delete []labelVoted;
// 	delete []probility;

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
	svm.loadModel(SVM_MODEL_FILE);
//	svm.crossValidation();
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
	QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
	double fSum = 0;
	double fSum5 = 0;

#ifdef USE_KNN
	OpenCVKNN knn;
	knn.train(m_textureTrain, m_labelMat, 50);
#endif
	QFile fileError("G:/data/68Data/error.txt");
	fileError.open(QFile::WriteOnly);
	QTextStream s(&fileError);
	for (int i = 0; i < fileList.size(); i++)
	{
		QString filePath = fileList.at(i).filePath();
		QString fileName = fileList.at(i).baseName();
		int iId = fileName.toInt();
		int iLabel = imgLabel.value(iId);
		cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
		FlowerFeatureExtractor f(mat);
		cv::Mat clrF = f.extractColor();
		cv::Mat shapeF = f.extractShape2();
		cv::Mat glcmF = f.extractGLCM();
		cv::Mat F;
//		F.push_back(clrF.reshape(1, clrF.cols));
//		F.push_back(shapeF.reshape(1, shapeF.cols));
		F.push_back(glcmF.reshape(1, glcmF.cols));
		F = F.reshape(1, 1);
		
		for (int c = 0; c < featureMinMax.cols; c++)
		{
			float fMin = featureMinMax.at<float>(0, c);
			float fMax = featureMinMax.at<float>(1, c);
			if (qAbs(fMax - fMin) < 0.000001)
				F.at<float>(0, c) = 1;
			else
				F.at<float>(0, c) = (F.at<float>(0, c) - fMin) / (fMax - fMin);
		}

#ifndef USE_KNN

		int *labelVoted = new int[svm.getClassCount()];
		double *probility = new double[svm.getClassCount()];
		svm.predict_probility(F, labelVoted, probility);
		if (labelVoted[0] == iLabel)
			fSum++;
		else
			s << filePath << endl;
		emit logout(tr("%1 ------ %2").arg(labelVoted[0]).arg(iLabel));
		for (int i = 0; i < 5; i++)
		{
			if (labelVoted[i] == iLabel)
			{
				fSum5++;
				break;
			}
		}
#endif		
#ifdef USE_KNN
		cv::Mat matLabel;
		knn.predict(F, matLabel);

		QList<int> vecLabel;
		for (int i = 0; i < matLabel.cols; i++)
			vecLabel.push_back(matLabel.at<float>(0, i));
		QList<int> vecUsed;
		while (vecLabel.size() > 0)
		{
			QMap<int, int> labelMap;
			for (int i = 0; i < 10; i++)
			{
				if (vecLabel.size() - 1 < i)
					break;
				int temp = vecLabel[i];
				if (!labelMap.contains(temp))
					labelMap[temp] = 0;
				else
					labelMap[temp]++;
			}

			QMap<int, int>::iterator itr = labelMap.begin();
			int iMax = 0;
			int iIndex = -1;
			while (itr != labelMap.end())
			{
				if (iMax < itr.value())
				{
					iMax = itr.value();
					iIndex = itr.key();
				}
				itr++;
			}
			if (iIndex >= 0)
				vecLabel.removeAll(iIndex);
			vecUsed.push_back(iIndex);
			if (vecUsed.size() >= 5)
				break;
		}
		if (vecUsed.size() == 0)
		{
			continue;
		}
		else
		{
			if (vecUsed[0] == iLabel)
				fSum++;
			for (int i = 0; i < 5; i++)
			{
				if (vecUsed[i] == iLabel)
				{
					fSum5++;
					break;
				}
			}
		}
#endif

//		cv::Mat labelF(1, 5, CV_32SC1);//		QSet<int> labelTrain;
//		cv::Mat prob(1, 5, CV_32FC1);
//		for (int i = 0; i < 5; i++)
//		{
//			labelF.at<int>(0, i) = labelVoted[i];
//			labelTrain.insert(labelVoted[i]);
//			prob.at<float>(0, i) = probility[i];
//		}

//		qDebug() << iLabel << "-->" << knn.predict(labelF, prob);
		
// 		cv::Mat trainData_top5;
// 		cv::Mat label_top5;
// 		std::vector<std::pair<double, int> > vecDis;
// 		int iDictionarySize = 500;
// 		cv::BOWKMeansTrainer bowTrainer(iDictionarySize, cv::TermCriteria(CV_TERMCRIT_ITER, 20,0.001), 3, cv::KMEANS_PP_CENTERS);
// 		cv::Mat matSift;
// 		for (int r = 0; r < m_labelMat.rows; r++)
// 		{
// 			int iLabel = m_labelMat.at<int>(r, 0);
// 			if (labelTrain.contains(iLabel))
// 			{
// 				QString filePath = m_vecFiles.at(r);
// 				cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
// 				FlowerFeatureExtractor ext(mat);
// 				cv::Mat sift = ext.extractSift();
// 				matSift.push_back(sift);
// //				trainData_top5.push_back(m_train.row(r));
// 				label_top5.push_back(iLabel);
// 				vecDis.push_back(std::pair<double, int>(cv::norm(m_train.row(r), F, cv::NORM_L2), iLabel));
// 			}
// 		}
// 		cv::Mat dictionary = bowTrainer.cluster(matSift);
// 		cv::Ptr<cv::DescriptorMatcher> matcher(new cv::FlannBasedMatcher);
// 		cv::Ptr<cv::FeatureDetector> detector(new cv::SiftFeatureDetector(500));
// 		cv::Ptr<cv::DescriptorExtractor> extractor(new cv::SiftDescriptorExtractor);    
// 		cv::BOWImgDescriptorExtractor bowDE(extractor,matcher);
// 		bowDE.setVocabulary(dictionary);
// 		for (int r = 0; r < m_labelMat.rows; r++)
// 		{
// 			int iLabel = m_labelMat.at<int>(r, 0);
// 			if (labelTrain.contains(iLabel))
// 			{
// 				QString filePath = m_vecFiles.at(r);
// 				cv::Mat mat = cv::imread(filePath.toLocal8Bit().data());
// 				std::vector<cv::KeyPoint> keypoints;        
// 				detector->detect(mat,keypoints);
// 				cv::Mat bowDescriptor;        
// 				bowDE.compute(mat, keypoints, bowDescriptor);
// 				trainData_top5.push_back(bowDescriptor);
// 			}
// 		}
// 		cv::Mat bowDescriptor;
// 		std::vector<cv::KeyPoint> keypoints;
// 		detector->detect(mat,keypoints);
// 		bowDE.compute(mat, keypoints, bowDescriptor);
// 		OpenCVLibSVM s;
// 		s.train(trainData_top5, label_top5, "I:/test.xml");
// //		s.crossValidation();
// 		s.predict_values(bowDescriptor, labelVoted);
// 		if (labelVoted[0] == iLabel)
// 			fSum++;
// 
// 
// 		delete []labelVoted;
// 		delete []probility;
	}
	fileError.close();

	emit logout(tr("Top1 Accurrate %1%").arg(fSum / fileList.size() * 100));
	emit logout(tr("Top5 Accurrate %1%").arg(fSum5 / fileList.size() * 100));
}