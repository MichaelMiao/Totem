#include "stdafx.h"
#include "ImageFolderLoader.h"
#include "../../designnet_core/designnetconstants.h"
#include "designnet/designnet_core/property/pathdialogproperty.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/intdata.h"
#include "Utils/fileutils.h"
#include <QIcon>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMultiMap>
#include <QMovie>
#include "Utils/opencvhelper.h"
#include "opencv2/nonfree/ocl.hpp"
#include "opencv2/ocl/ocl.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/ml/ml.hpp"

using namespace DesignNet;
#define SIFTFEATURE_COUNT		100
#define OUTPUT_IMAGE			_T("Output Image")
#define OUTPUT_FILE_PATH		_T("Image File Path")

const QString strTrainPath = "I:/MD/flower/flowers/train82_10";
const QString strValidationPath = "I:/MD/flower/flowers/validation/";
const QString strTestPath = "I:/MD/flower/flowers/test/";


namespace InputLoader{

const char ImageFolderIcon[] = ":/InputLoader/images/add_folder.png";
ImageFolderLoader::ImageFolderLoader( DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/ )
	: Processor(space, parent),
	m_iCurIndex(0)
{
	setName(tr("Image Folder Loader"));
	setIcon((QLatin1String(ImageFolderIcon)));
	addPort(Port::OUT_PORT, DATATYPE_IMAGE, OUTPUT_IMAGE);
	addPort(Port::OUT_PORT, DATATYPE_STRING, OUTPUT_FILE_PATH);
}

ImageFolderLoader::~ImageFolderLoader(void)
{
}

Processor* ImageFolderLoader::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new ImageFolderLoader(space);
}

QString ImageFolderLoader::title() const
{
	return tr("Image Folder Loader");
}

QString ImageFolderLoader::category() const
{
	return tr("Loader");
}

void ImageFolderLoader::LoadSiftFeature()
{
	cv::Mat features;
	emit logout("start train..");
	int iCount = 0;
	foreach (QString file, m_filePaths)
	{
		QFileInfo fInfo(file);
		QString fileName = fInfo.fileName();
		emit logout(tr("processing %1").arg(fileName));
		std::string str = file.toLocal8Bit().data();
		QString strSrcFile = tr("I:/MD/flower/flowers/train82_10/") + fileName;
		QString strMaskFile = tr("I:/MD/flower/flowers/mask/") + fileName;
		QString strYmlFile	= tr("I:/MD/flower/flowers/sift_yml/%1.yml").arg(fInfo.baseName());
		cv::Mat mat = cv::imread(strSrcFile.toLocal8Bit().data());
		cv::Mat matMask = cv::imread(strMaskFile.toLocal8Bit().data(), CV_LOAD_IMAGE_GRAYSCALE);
		cv::Mat descriptor;
		SiftMat(mat, matMask, descriptor);
		cv::FileStorage fs(strYmlFile.toLocal8Bit().data(), cv::FileStorage::WRITE);
		fs << "SIFT" << descriptor;
		fs.release();
	}
}

QMultiMap<int, int> datas; // 所有的数据
QMultiMap<int, int> datas_valid; // 所有有效数据

bool comp(int iFirst, int iSec)
{
	int iCount1 = datas_valid.count(iFirst);
	int iCount2 = datas_valid.count(iSec);
	return iCount1 >= iCount2;
}

void ImageFolderLoader::LoadLabel()
{
	datas.clear();
	datas_valid.clear();
	const QString fileName = tr("I:\\MD\\flower\\data.txt");
	QFile file(fileName);
	if(file.open(QFile::ReadOnly))
	{
		QTextStream s(&file);
		int iCount = 0;
		while (!s.atEnd())
		{
			s.readLine();
			iCount++;
		}
		int *vecLabels = new int[iCount];
		memset(vecLabels, 0, sizeof(int) * iCount);
		int iNum, iLabel;
		s.seek(0);
		while (!s.atEnd())
		{
			QString strTemp = s.readLine();
			qDebug() << strTemp;
			QTextStream strLine(&strTemp);
			strLine >> iNum >> iLabel;
			datas.insert(iLabel, iNum);
		}
		
		delete []vecLabels;
		file.close();
		QDir dir(strTrainPath);
		QStringList nameFilters;
		nameFilters << "*.jpg";
		QFileInfoList infoList = dir.entryInfoList(nameFilters, 
			QDir::Files | QDir::Readable);
		foreach(QFileInfo info, infoList)
		{
			QString fileValid	= (info.absoluteFilePath());
			QString fileName	= info.fileName();
			int iNum = fileName.mid(7, 5).toInt();
			datas_valid.insert(datas.key(iNum), iNum);
		}
	}
	QFile ofile("I:/MD/flower/flowers/count.txt");
	if (ofile.open(QFile::WriteOnly))
	{
		QTextStream stream(&ofile);

		QList<int> datasKeys = datas_valid.keys();
		qSort(datasKeys.begin(), datasKeys.end(), comp);
		foreach (int iLabel, datasKeys)
		{
			QList<int> nums = datas_valid.values(iLabel);
			stream << iLabel << "\t" << nums.size() << endl;
		}
		ofile.close();
	}
}

void ImageFolderLoader::LoadData()
{
	LoadLabel();
	QFile ofile("I:/MD/flower/flowers/count.txt");
	if (ofile.open(QFile::WriteOnly))
	{
		QTextStream stream(&ofile);

		QSet<int> dataKeys = datas.keys().toSet();
		QList<int> dataList = dataKeys.toList();
		qSort(dataList.begin(), dataList.end(), comp);
		foreach (int iLabel, dataList)
		{
			QList<int> nums = datas.values(iLabel);
			stream << iLabel << "\t" << nums.size() << endl;
		}
		ofile.close();
	}
}


void ImageFolderLoader::SaveBinary()
{
	QString strOutputPath = "I:/MD/flower/flowers/mask/";
	foreach (QString file, m_filePaths)
	{
		QFileInfo fInfo(file);
		QString fileName = fInfo.fileName();
		std::string str = file.toLocal8Bit().data();
		cv::Mat mat = cv::imread(str);
		if(mat.data)
		{
			cv::medianBlur(mat, mat, 5);
			cv::Mat grayMat(mat.rows, mat.cols, CV_8UC1);
			for (int i = 0; i < mat.rows; ++i)
			{
				uchar *p = mat.ptr<uchar>(i);
				uchar *pout = grayMat.ptr<uchar>(i);
				int iDelta = 0;
				for (int j = 0, m = 0; j < mat.cols; ++j)
				{
					iDelta = (p[m] - 254) * (p[m] - 254) + (p[m + 1]) * (p[m + 1]) + (p[m + 2]) * (p[m + 2]);
					if (iDelta < 10)
					{
						pout[j] = 0;
					}
					else
						pout[j] = 255;
					m += 3;
				}
			}
			std::vector<std::vector<cv::Point> > countours;
			cv::findContours(grayMat.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			size_t counts = countours.size();
			if(counts <= 0)
				continue;/// 找到的轮廓数不能为0

			double maxarea = 0;
			cv::Rect maxrect(0, 0, 2, 2);
			size_t max_index = -1;
			for (size_t i = 0; i < counts ; ++i)
			{
				if (maxarea < countours.at(i).size())
				{
					maxarea = countours.at(i).size();
					max_index = i;
				}
			}

			if (max_index != -1)
			{
				cv::Mat out = cv::Mat::zeros(mat.rows, mat.cols, CV_8UC1);

				cv::drawContours(out, countours, max_index, cv::Scalar(255), -1);
				cv::imwrite((strOutputPath + fileName).toLocal8Bit().data(), out);
			}
		}
	}
}

void ImageFolderLoader::SaveContours()
{
	QString strOutputPath = "I:/MD/flower/flowers/mask/";
	foreach (QString file, m_filePaths)
	{
		QFileInfo fInfo(file);
		QString fileName = fInfo.fileName();
		std::string str = file.toLocal8Bit().data();
		cv::Mat mat = cv::imread(str, CV_LOAD_IMAGE_GRAYSCALE);
		int iChannels = mat.channels();
		std::vector<std::vector<cv::Point> > countours;
		cv::findContours(mat.clone(), countours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		size_t counts = countours.size();
		if(counts <= 0)
			continue;/// 找到的轮廓数不能为0

		double maxarea = 0;
		cv::Rect maxrect(0, 0, 2, 2);
		size_t max_index = -1;
		for (size_t i = 0; i < counts ; ++i)
		{
			if (maxarea < countours.at(i).size())
			{
				maxarea = countours.at(i).size();
				max_index = i;
			}
		}

		if (max_index != -1)
		{
			cv::Mat out = cv::Mat::zeros(mat.rows, mat.cols, CV_8UC1);
			
			cv::drawContours(out, countours, max_index, cv::Scalar(255), -1);
			QImage img = Utils::OpenCVHelper::Mat2QImage(out);
			img.save(strOutputPath + fileName);
		}
	}
}

void ImageFolderLoader::SaveFinal()
{
	QString strBinary = "I:/MD/flower/flowers/contours/";
	QString strSrc	  = "I:/MD/flower/flowers/train82/";
	QString strOut	  = "I:/MD/flower/flowers/train82_fixed/";
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	QDir dir(strSrc);
	QFileInfoList infoList = dir.entryInfoList(nameFilters, QDir::Files | QDir::Readable);

	foreach(QFileInfo info, infoList)
	{
		QString name = info.fileName();
		QString outFile = strOut + name;
		cv::Mat mat = cv::imread(info.absoluteFilePath().toLocal8Bit().data());
		cv::Mat matMask = cv::imread(QString(strBinary + name).toLocal8Bit().data(), CV_LOAD_IMAGE_GRAYSCALE);
		cv::Mat out(mat.rows, mat.cols, CV_8UC3, cv::Scalar(0xfe, 0, 0));
		mat.copyTo(out, matMask);
		cv::imwrite(outFile.toLocal8Bit().data(), out);
	}
}


void ImageFolderLoader::SiftMat(cv::Mat &mat, cv::Mat & mask, cv::Mat &descriptor)
{
	cv::SIFT ext(SIFTFEATURE_COUNT);
	std::vector<cv::KeyPoint> keyPoints;
	ext(mat, mask, keyPoints, descriptor);
}

int ImageFolderLoader::MaskMat( cv::Mat &mat, cv::Mat& mask )
{
	cv::medianBlur(mat, mat, 5);
	cv::Mat grayMat(mat.rows, mat.cols, CV_8UC1);
	int iCount = 0;
	for (int i = 0; i < mat.rows; ++i)
	{
		uchar *p = mat.ptr<uchar>(i);
		uchar *pout = grayMat.ptr<uchar>(i);
		int iDelta = 0;
		for (int j = 0, m = 0; j < mat.cols; ++j)
		{
			iDelta = (p[m] - 254) * (p[m] - 254) + (p[m + 1]) * (p[m + 1]) + (p[m + 2]) * (p[m + 2]);
			if (iDelta < 10)
			{
				pout[j] = 0;
			}
			else
			{
				pout[j] = 255;
				iCount++;
			}
			m += 3;
		}
	}
	mask = grayMat;
	return iCount;
}

bool ImageFolderLoader::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
// 	cv::Mat mat1 = cv::imread("I:/MD/1.bmp");
// 	cv::Mat matR = cv::imread("I:/MD/R.bmp");
// 	cv::SiftFeatureDetector sift;
// 	cv::Mat matDescript1, matDescriptR;
// 	std::vector<cv::KeyPoint> vecRKeyPoint1, vecRKeyPointR;
// 	sift.detect(mat1, vecRKeyPoint1);
// 	sift.detect(matR, vecRKeyPointR);
// 	sift.compute(mat1, vecRKeyPoint1, matDescript1);
// 	sift.compute(matR, vecRKeyPointR, matDescriptR);
// 	std::vector<cv::DMatch> matches;
// 	cv::BFMatcher bfmacher;
// 	bfmacher.match(matDescript1, matDescriptR, matches);
// 	cv::Mat mm(mat1.rows, mat1.cols, CV_8UC1);
// 	for (int i = 0; i < matches.size(); i++)
// 	{
// 		cv::KeyPoint kp = vecRKeyPoint1.at(matches.at(i).trainIdx);
// 		mm.at<uchar>(kp.pt.y, kp.pt.x) = 255;
// 	}
// 	cv::imwrite("I:/res.bmp", mm);
// 	//画出匹配结果  
// 	cv::Mat img_matches;  
// 	//红色连接的是匹配的特征点对，绿色是未匹配的特征点  
// 	cv::drawMatches(mat1, vecRKeyPoint1,matR, vecRKeyPointR, matches, img_matches,  
// 		cv::Scalar::all(-1)/*CV_RGB(255,0,0)*/, CV_RGB(0,255,0), cv::Mat(), 2);
// 	cv::imwrite("I:/mm.bmp", img_matches);
//	cv::initModule_nonfree();
//	SVM_SIFT();
//	TestTrain();

//	LoadData();
//	SaveBinary();

//	SaveContours();
//	SaveFinal();
//	BOWCluster();
//	RepresentBOW();
//	SVMTrain();
//	SVMTest();
//	PrepareData();
//	SVM_C();
//	return true;

	emit logout(tr("ImageFolderLoader Processing"));
	ProcessResult pr;
	if (m_iCurIndex >= m_filePaths.size())
	{
		pr.m_bSucessed = true;
		pr.m_bNeedLoop = false;
		future.reportResult(pr, 0);
		return true;
	}
	pr.m_bNeedLoop = true;
	QString file = m_filePaths.at(m_iCurIndex);
	std::string str = file.toLocal8Bit().data();
	cv::Mat mat = cv::imread(str);
	if(!mat.data)
	{
		emit logout(tr("load %1 failed").arg(file));
		pr.m_bSucessed = false;
		pr.m_bNeedLoop = false;
		future.reportResult(pr, 0);
		return false;
	}
	else
	{
		emit logout(tr("loading %1...").arg(file));
		m_imageData.setImageData(mat);
		m_imageData.setIndex(m_iCurIndex++);
		DataType dt = DATATYPE_MATRIX;
		if (mat.type() == CV_8UC1)
			dt = DATATYPE_GRAYIMAGE;
		else if (mat.type() == CV_8UC3)
			dt = DATATYPE_8UC3IMAGE;

		pushData(&m_imageData, OUTPUT_IMAGE);
	}
	future.reportResult(pr, 0);
	return true;
}

void ImageFolderLoader::setPath( const QString &p )
{
	QWriteLocker locker(&m_lock);
	m_folderPath = p;
}

QString ImageFolderLoader::path() const
{
	QReadLocker locker(&m_lock);
	return m_folderPath;
}

bool ImageFolderLoader::prepareProcess()
{
	if(m_folderPath.isEmpty())
	{
		emit logout(tr("Property %1 is not valid.").arg(m_folderPath));
		return false;
	}
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	QDir dir(m_folderPath);
	QFileInfoList infoList = dir.entryInfoList(nameFilters, 
		QDir::Files | QDir::Readable);
	
	foreach(QFileInfo info, infoList)
		m_filePaths << info.absoluteFilePath();

	ProcessData data;
	data.dataType = DATATYPE_INT;
	data.processorID = m_id;
	pushData(data, OUTPUT_IMAGE);

	return true;
}

bool ImageFolderLoader::finishProcess()
{
	m_iCurIndex = 0;
	return true;
}

void ImageFolderLoader::BOWCluster()
{
	cv::ocl::DevicesInfo dinfo;
	cv::ocl::getOpenCLDevices(dinfo);
	QSet<int> labels = datas_valid.keys().toSet();
	char outputPath[255];
	char inputPath[255];
	cv::FileStorage fs("I:/MD/flower/flowers/bow_yml/bowkmean.xml", cv::FileStorage::WRITE);
	int iCount = 0;
	cv::Mat features;
	foreach (int iLabel, labels)
	{
		QList<int> nums = datas_valid.values(iLabel);
		emit logout(tr("loading"));
		foreach (int iNum, nums)
		{
			sprintf(inputPath, "I:/MD/flower/flowers/sift_yml/segmim_%05d.yml", iNum);
			cv::FileStorage fs(inputPath, cv::FileStorage::READ);
			cv::Mat mat;
			fs["SIFT"] >> mat;
			emit logout(tr("push %1").arg(iNum));
			fs.release();
			features.push_back(mat);
		}
	}
	emit logout(tr("cluster..."));
	int iDictionarySize = 1000;
	cv::Mat dictionary;
	cv::BOWKMeansTrainer bowTrainer(iDictionarySize, cv::TermCriteria(CV_TERMCRIT_ITER, 20,0.001), 3, cv::KMEANS_PP_CENTERS);
	dictionary = bowTrainer.cluster(features);
	fs << "bow" << dictionary;
	fs.release();
}

void ImageFolderLoader::RepresentBOW()
{
	cv::Mat dictionary;
	cv::FileStorage fs("I:/MD/flower/flowers/bow_yml/bowkmean.xml", cv::FileStorage::READ);
	fs["bow"] >> dictionary;
	fs.release();
	QSet<int> labels = datas_valid.keys().toSet();
	cv::Ptr<cv::DescriptorMatcher> matcher(new cv::FlannBasedMatcher);
	cv::Ptr<cv::FeatureDetector> detector(new cv::SiftFeatureDetector(500));
	cv::Ptr<cv::DescriptorExtractor> extractor(new cv::SiftDescriptorExtractor);    
	cv::BOWImgDescriptorExtractor bowDE(extractor,matcher);

	bowDE.setVocabulary(dictionary);
	char inputPath[255];
	cv::FileStorage fsfesture("I:/MD/flower/flowers/bow_yml/bowfeature.xml", cv::FileStorage::WRITE);
	foreach (int iLabel, labels)
	{
		QList<int> nums = datas_valid.values(iLabel);
		emit logout(tr("loading"));
		foreach (int iNum, nums)
		{
			sprintf(inputPath, "I:/MD/flower/flowers/train82_10/segmim_%05d.jpg", iNum);
			cv::Mat img = cv::imread(inputPath);
			std::vector<cv::KeyPoint> keypoints;        
			detector->detect(img,keypoints);
			cv::Mat bowDescriptor;        
			bowDE.compute(img, keypoints, bowDescriptor);
			char lab[255];
			sprintf(lab, "bow_%05d", iNum);
			fsfesture << lab << bowDescriptor;
		}
	}
	fsfesture.release();
}

void ImageFolderLoader::SVMTrain()
{
	cv::FileStorage fsfesture("I:/MD/flower/flowers/bow_yml/bowfeature.xml", cv::FileStorage::READ);
	cv::FileNode node = fsfesture.root();
	cv::FileNodeIterator itr = node.begin();
	int iNum;
	cv::Mat trainData;
	cv::Mat labels;
	while (itr != node.end())
	{
		std::string strName = (*itr).name();

		cv::Mat bowDescriptor;        
		(*itr) >> bowDescriptor;
		trainData.push_back(bowDescriptor);
		sscanf(strName.c_str(), "bow_%d", &iNum);
		qDebug() << strName.c_str();
		labels.push_back(datas.key(iNum));
		itr++;
	}
	cv::SVMParams param;
	param.kernel_type = CvSVM::RBF;
	param.degree = 3;
	param.gamma = 1.0 / 128;
	param.term_crit = cv::TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 1000, FLT_EPSILON);
	CvSVM svm;
	svm.train_auto(trainData, labels, cv::Mat(), cv::Mat(), param);
	svm.save("I:/MD/flower/flowers/svm.xml");
}

void ImageFolderLoader::SVMTest()
{
	CvSVM svm;
	svm.load("I:/MD/flower/flowers/svm.xml");

	cv::Ptr<cv::DescriptorMatcher> matcher(new cv::FlannBasedMatcher);
	cv::Ptr<cv::FeatureDetector> detector(new cv::SiftFeatureDetector(SIFTFEATURE_COUNT));
	cv::Ptr<cv::DescriptorExtractor> extractor(new cv::SiftDescriptorExtractor);

	cv::FileStorage fs("I:/MD/flower/flowers/bow_yml/bowkmean.xml", cv::FileStorage::READ);
	cv::Mat dictionary;
	fs["bow"] >> dictionary;

	cv::BOWImgDescriptorExtractor bowDE(extractor,matcher);
	bowDE.setVocabulary(dictionary);
	int iCount = 0;
	int iCorrect = 0;
	foreach (QString file, m_filePaths)
	{
		QFileInfo fInfo(file);
		QString fileName = fInfo.fileName();
		QString s = fileName.mid(7, 5);
		int iNum = s.toInt();
		int iLabel = datas.key(iNum);
		if (!datas_valid.contains(iLabel))
			continue;

		iCount++;
		emit logout(tr("processing %1").arg(fileName));
		cv::Mat mat = cv::imread(file.toLocal8Bit().data());
		cv::Mat matMask;
		MaskMat(mat, matMask);
		
		std::vector<cv::KeyPoint> keypoints;        
		detector->detect(mat, keypoints, matMask);
		cv::Mat bowDescriptor;        
		bowDE.compute(mat, keypoints, bowDescriptor);
		float fResponse = svm.predict(bowDescriptor);
		
		if (iLabel == fResponse)
		{
			iCorrect++;
		}
		emit logout(tr("%1 -- %2 -- %3 %4%").arg(fResponse).arg(iLabel).arg(iNum).arg(iCorrect * 100.0/ iCount));
	}

	if (iCount > 0)
		emit logout(tr("%1").arg(iCorrect * 1.0/ iCount));
}

void ImageFolderLoader::PrepareData()
{
	LoadLabel();
	QString strFolder = "I:/MD/flower/flowers/train82_fixed/";
	QString strSrc = "I:/software/icon/flowers/segmim/";
	QString strFolderValidation = "I:/MD/flower/flowers/validation/";
	QString strFolderOut = "I:/MD/flower/flowers/test/";
	char dataFilePath[255];
	QDir dir(strFolder);
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	QFileInfoList infoList = dir.entryInfoList(nameFilters, 
		QDir::Files | QDir::Readable);
	QDir dirSrc("strFolder");
	QFileInfoList infoListSrc = dirSrc.entryInfoList(nameFilters,
		QDir::Files | QDir::Readable);

	QSet<int> dataLabels = datas.keys().toSet();
	QMultiMap<int, int> usedData;
	foreach(QFileInfo info, infoList)
	{
		QString sFileName = info.fileName();
		QString s = sFileName.mid(7, 5);
		int iNum = s.toInt();
		int iLabel = datas.key(iNum);
		usedData.insert(iLabel, iNum);
	}
	QSet<int> usedLabels = usedData.keys().toSet();
	foreach (int iLabel, usedLabels)
	{
		QList<int> nums = datas.values(iLabel);
		QList<int> usednums = usedData.values(iLabel);
		int iCountValidation = 0;
		int iCountTest		= 0;
		qDebug() << nums;
		foreach (int inum, nums)
		{
			if (usednums.contains(inum))
				continue;
			sprintf(dataFilePath, "segmim_%05d.jpg", inum);

			if (iCountValidation < 20)
			{
				QString s = strSrc + dataFilePath;
				QString t = strFolderValidation + dataFilePath;
				bool b = QFile::copy(s, t);
				iCountValidation++;
			}
			else if (iCountTest < 10)
			{
				QFile::copy(strSrc + dataFilePath, strFolderOut + dataFilePath);
				iCountTest++;
			}
		}
	}
	return;
// 	QMap<int, int> labelCount;
// 	
// 	foreach (QFileInfo infoSrc, infoListSrc)
// 	{
// 		if (strFileNames.contains(infoSrc.fileName()))
// 			continue;
// 		QString sFileName = infoSrc.fileName();
// 		QString s = sFileName.mid(7, 5);
// 		int iNum = s.toInt();
// 		int iLabel = datas.key(iNum);
// 		
// 	}
// 
// 	foreach(QFileInfo info, infoList)
// 	{
// 		QString sFileName = info.fileName();
// 		QString s = sFileName.mid(7, 5);
// 		int iNum = s.toInt();
// 		int iLabel = datas.key(iNum);
// 
// 		
// 
// 		if (!labelCount.contains(iLabel))
// 			labelCount[iLabel] = 0;
// 		
// 		labelCount[iLabel]++;
// 		if (labelCount[iLabel] <= 20)
// 		{
// 			QString sOutputFile = strFolderOut + sFileName;
// 			QFile::copy(info.absoluteFilePath(), sOutputFile);
// 			QFile::remove(info.absoluteFilePath());
// 		}
// 	}
}

void ImageFolderLoader::SVM_C()
{
	LoadLabel();
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	QDir dir(strTrainPath);
	QFileInfoList infoList = dir.entryInfoList(nameFilters, QDir::Files | QDir::Readable);
	cv::Mat featureKMean;
	int channel[] = { 0, 1 };
	int histogramSize[] = { 255, 255 };
	float h_ranges[] = { 0, 256 };
	float s_ranges[] = { 0, 180 };
	const float* ranges[] = { h_ranges, s_ranges };
	foreach(QFileInfo info, infoList)
	{
		QString sFileName = info.fileName();
		QString sFilePath = info.absoluteFilePath();
		cv::Mat mat = cv::imread(sFilePath.toLocal8Bit().data());
		emit logout(tr("processing..%1").arg(sFileName));
		cv::Mat matHSV(mat.rows, mat.cols, CV_8UC3);
		cv::Mat mask;
		int iPixCount = MaskMat(mat, mask);		
		cv::cvtColor(mat, matHSV, cv::COLOR_BGR2HSV);
		cv::Mat histogramMat;
		cv::Mat histogramNorm;
		cv::calcHist(&matHSV, 1, channel, mask, histogramMat, 2, histogramSize, ranges);
		cv::normalize(histogramMat, histogramNorm, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

//		featureKMean.push_back(feature);
	}
	emit logout(tr("cluster"));
	cv::BOWKMeansTrainer bow(200, cv::TermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, FLT_EPSILON), 3, cv::KMEANS_PP_CENTERS);
	cv::Mat dictionary = bow.cluster(featureKMean);
	cv::FileStorage fs("I:/MD/flower/flowers/cluster/color_dictionary.yml", cv::FileStorage::WRITE);
	fs << "color_dictionary" << dictionary;
	fs.release();	
}

void ImageFolderLoader::SVM_SIFT()
{
	LoadLabel();
//	LoadSiftFeature();
	BOWCluster();
	SVMTrain();
	SVMTest();
}

void ImageFolderLoader::SVM_HOG()
{

}

void ImageFolderLoader::serialize(Utils::XmlSerializer& s) const
{
	Processor::serialize(s);
	s.serialize("path", m_folderPath);
}

void ImageFolderLoader::deserialize(Utils::XmlDeserializer& s)
{
	s.deserialize("path", m_folderPath);
	Processor::deserialize(s);
}

}//!< namespace InputLoader