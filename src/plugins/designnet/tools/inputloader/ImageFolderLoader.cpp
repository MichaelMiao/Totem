#include "stdafx.h"
#include "ImageFolderLoader.h"
#include "designnet/designnet_core/property/pathdialogproperty.h"
#include "designnet/designnet_core/data/imagedata.h"
#include "designnet/designnet_core/data/intdata.h"
#include "Utils/fileutils.h"
#include <QIcon>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMultiMap>
#include "Utils/opencvhelper.h"
#include "opencv2/nonfree/ocl.hpp"
#include "opencv2/ocl/ocl.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/ml/ml.hpp"

using namespace DesignNet;
namespace InputLoader{

const char ImageFolderIcon[] = ":/InputLoader/images/add_folder.png";
ImageFolderLoader::ImageFolderLoader( DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/ )
	: Processor(space, parent),
	m_iCurIndex(0)
{
	setName(tr("Image Folder Loader"));
	setIcon((QLatin1String(ImageFolderIcon)));
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

void ImageFolderLoader::TestTrain()
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
		QString strTestFile = tr("F:/MD/flower/flowers/train82_fixed/") + fileName;
		QString strMaskFile = tr("F:/MD/flower/flowers/mask/") + fileName;
		QString strYmlFile	= tr("F:/MD/flower/flowers/sift_yml/%1.yml").arg(fInfo.baseName());
		cv::Mat mat = cv::imread(strTestFile.toLocal8Bit().data());
		cv::Mat matMask = cv::imread(strMaskFile.toLocal8Bit().data(), CV_LOAD_IMAGE_GRAYSCALE);
		cv::Mat descriptor;
		SiftMat(mat, matMask, descriptor);
		cv::FileStorage fs(strYmlFile.toLocal8Bit().data(), cv::FileStorage::WRITE);
		fs << "SIFT" << descriptor;
		fs.release();
	}
	
	emit logout("cluster start...");
	int iDictionarySize = 200;
	cv::BOWKMeansTrainer bow(iDictionarySize, cv::TermCriteria(CV_TERMCRIT_ITER,100,0.001), 3, cv::KMEANS_PP_CENTERS);
	cv::Mat dictionary = bow.cluster(features);
	cv::FileStorage fs("D:/test.yml", cv::FileStorage::WRITE);
	fs << dictionary;
	fs.release();
	emit logout("cluster finish");
// 	for(int i = 0; i < m_filePaths.size(); i++)
// 	{
// 		QString file = m_filePaths[i];
// 		std::string str = file.toLocal8Bit().data();
// 		cv::Mat matSrc = cv::imread(str);	// 读图片
// 		cv::Mat grayMat, hsvMat;
// 		cv::cvtColor(matSrc, grayMat, CV_RGB2GRAY);
// 		cv::cvtColor(matSrc, hsvMat, CV_RGB2HSV);
// 
// 		int hbins = 30, sbins = 32;
// 		int histSize[] = { hbins, sbins };
// 		int channels[] = {0, 1};
// 		float hranges[] = { 0, 180 };
// 		// saturation varies from 0 (black-gray-white) to
// 		// 255 (pure spectrum color)
// 		float sranges[] = { 0, 256 };
// 		const float* ranges[] = { hranges, sranges };
// 		cv::calcHist( &hsvMat, 1, channels, cv::Mat(), // do not use mask
// 			m_hist, 2, histSize, ranges,
// 			true, // the histogram is uniform
// 			false );
// 		cv::Mat feature(1, 5, CV_32FC1);
// 		int hmax = 0, smax = 0, hmax2 = 0, smax2 = 0;
// 		float max_value = 0, max2_value = 0;
// 
// 		for(int i = 0; i < hbins; ++i)
// 		{
// 			for(int j = 0; j < sbins; ++j)
// 			{
// 				float f = m_hist.at<float>(i, j);
// 
// 				if(f > max2_value )/// 大于次大值
// 				{
// 					if(f > max_value)/// 大于最大值
// 					{
// 						hmax = i;
// 						smax = j;
// 						max_value = f;
// 					}
// 					else
// 					{
// 						hmax2 = i;
// 						smax2 = j;
// 						max2_value = f;
// 					}
// 				}
// 			}
// 		}
// 		feature.at<float>(0) = hmax;
// 		feature.at<float>(1) = smax;
// 		feature.at<float>(2) = hmax2;
// 		feature.at<float>(3) = smax2;
// 		feature.at<float>(4) = max_value/(hsvimage.rows * hsvimage.cols + 1);
// 	}
}

QMultiMap<int, int> datas; // 所有的数据
QMap<int, int> datas_valid; // 所有有效数据

bool comp(int iFirst, int iSec)
{
	int iCount1 = datas.count(iFirst);
	int iCount2 = datas.count(iSec);
	return iCount1 >= iCount2;
}

void ImageFolderLoader::LoadLabel()
{
	datas.clear();
	datas_valid.clear();
	const QString fileName = tr("F:\\MD\\flower\\data.txt");
	const QString validName = tr("F:/MD/flower/flowers/count.txt");
	QFile file(fileName);
	QFile fileValid(validName);
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
			vecLabels[iNum - 1] = iLabel;
		}
		foreach (QString str, m_filePaths)
		{
			QFileInfo fInfo(str);
			str = fInfo.fileName();
			QString s = str.mid(7, 5);
			int iNum = s.toInt();
			int iLabel = vecLabels[iNum - 1];
			datas.insert(iLabel, iNum);
		}
		delete []vecLabels;
		file.close();
		if (fileValid.open(QFile::ReadOnly))
		{
			QTextStream stream(&fileValid);
			int iCount, iLabel;
			while (!stream.atEnd())
			{
				QString strTemp = stream.readLine();
				qDebug() << strTemp;
				QTextStream strLine(&strTemp);
				strLine >> iLabel >> iCount;
				datas_valid[iLabel] = iCount;
			}

			fileValid.close();
		}
	}

}

void ImageFolderLoader::LoadData()
{
	LoadLabel();
	QFile ofile("F:/MD/flower/flowers/count.txt");
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
	QString strOutputPath = "F:/MD/flower/flowers/contours/";
	foreach (QString file, m_filePaths)
	{
		QFileInfo fInfo(file);
		QString fileName = fInfo.fileName();
		std::string str = file.toLocal8Bit().data();
		cv::Mat mat = cv::imread(str);
		if(mat.data)
		{
			cv::medianBlur(mat, mat, 5);
//			cv::imwrite("F:/MD/flower/flowers/test.jpg", mat);
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
			cv::imwrite(QString(strOutputPath + fileName).toLocal8Bit().data(), grayMat);
		}
	}
}

void ImageFolderLoader::SaveContours()
{
	QString strOutputPath = "F:/MD/flower/flowers/mask/";
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
	QString strBinary = "F:/MD/flower/flowers/contours/";
	QString strSrc	  = "F:/MD/flower/flowers/train82/";
	QString strOut	  = "F:/MD/flower/flowers/train82_fixed/";
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
	cv::SIFT ext(200);
	std::vector<cv::KeyPoint> keyPoints;
	ext(mat, mask, keyPoints, descriptor);
}

void ImageFolderLoader::MaskMat( cv::Mat &mat, cv::Mat& mask )
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
	mask = grayMat;
}

bool ImageFolderLoader::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	cv::initModule_nonfree();

//	TestTrain();

//	LoadData();
//	SaveBinary();
//	SaveContours();
//	SaveFinal();
//	BOWCluster();
//	RepresentBOW();
//	SVMTrain();
//	SVMTest();
	PrepareData();
	return true;

// 	emit logout(tr("ImageFolderLoader Processing"));
// 	ProcessResult pr;
// 	if (m_iCurIndex >= m_filePaths.size())
// 	{
// 		pr.m_bSucessed = true;
// 		pr.m_bNeedLoop = false;
// 		future.reportResult(pr);
// 		return true;
// 	}
// 	pr.m_bNeedLoop = true;
// 	QString file = m_filePaths.at(m_iCurIndex);
// 	std::string str = file.toLocal8Bit().data();
// 	cv::Mat mat = cv::imread(str);
// 	if(!mat.data)
// 	{
// 		emit logout(tr("load %1 failed").arg(file));
// 		pr.m_bSucessed = false;
// 		pr.m_bNeedLoop = false;
// 		future.reportResult(pr);
// 		return false;
// 	}
// 	else
// 	{
// 		emit logout(tr("loading %1...").arg(file));
// 		m_imageData.setImageData(mat);
// 		m_imageData.setIndex(m_iCurIndex++);
// 		pushData(QVariant::fromValue((IData*)&m_imageData), "ImageData");
// 	}
// 	future.reportResult(pr);
// 	return true;
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
	m_folderPath = QString::fromUtf8("F:/MD/flower/flowers/train/");
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
	data.strLabel = "Outputcount";
	data.processorID = m_id;
	pushData(data);

	return true;
}

bool ImageFolderLoader::finishProcess()
{
	m_iCurIndex = 0;
	return true;
}

QList<ProcessData> ImageFolderLoader::dataProvided()
{
	QList<ProcessData> ret;
	ProcessData pd;
	pd.dataType = DATATYPE_INT;
	pd.processorID = m_id;
	pd.strLabel = "ImageCount";
	ret.push_back(pd);
	pd.dataType = DATATYPE_STRING;
	pd.strLabel = "FolderPath";
	ret.push_back(pd);
	pd.dataType = DATATYPE_8UC3IMAGE;
	pd.strLabel = "ImageData";
	ret.push_back(pd);
	return ret;
}

void ImageFolderLoader::BOWCluster()
{
	LoadLabel(); // 载入标签
	cv::ocl::DevicesInfo dinfo;
	cv::ocl::getOpenCLDevices(dinfo);
	QSet<int> labels = datas.keys().toSet();
	char outputPath[255];
	char inputPath[255];
	cv::FileStorage fs("F:/MD/flower/flowers/bow_yml/bowkmean.xml", cv::FileStorage::WRITE);
	int iCount = 0;
	cv::Mat features;
	foreach (int iLabel, labels)
	{
		QList<int> nums = datas.values(iLabel);
		emit logout(tr("loading"));
		foreach (int iNum, nums)
		{
			sprintf(inputPath, "F:/MD/flower/flowers/sift_yml/segmim_%05d.yml", iNum);
			cv::FileStorage fs(inputPath, cv::FileStorage::READ);
			cv::Mat mat;
			fs["SIFT"] >> mat;
			emit logout(tr("push %1").arg(iNum));
			fs.release();
			features.push_back(mat);
		}
	}
	emit logout(tr("cluster..."));
	int iDictionarySize = 200;
	cv::Mat dictionary;
	cv::BOWKMeansTrainer bowTrainer(iDictionarySize, cv::TermCriteria(CV_TERMCRIT_ITER,100,0.001), 3, cv::KMEANS_PP_CENTERS);
	dictionary = bowTrainer.cluster(features);
	fs << "bow" << dictionary;
	fs.release();
}

void ImageFolderLoader::RepresentBOW()
{
	LoadLabel();
	cv::Mat dictionary;
	cv::FileStorage fs("F:/MD/flower/flowers/bow_yml/bowkmean.xml", cv::FileStorage::READ);
	fs["bow"] >> dictionary;
	fs.release();
	QSet<int> labels = datas.keys().toSet();
	cv::Ptr<cv::DescriptorMatcher> matcher(new cv::FlannBasedMatcher);
	cv::Ptr<cv::FeatureDetector> detector(new cv::SiftFeatureDetector(200));
	cv::Ptr<cv::DescriptorExtractor> extractor(new cv::SiftDescriptorExtractor);    
	cv::BOWImgDescriptorExtractor bowDE(extractor,matcher);

	bowDE.setVocabulary(dictionary);
	char inputPath[255];
	cv::FileStorage fsfesture("F:/MD/flower/flowers/bow_yml/bowfeature.xml", cv::FileStorage::WRITE);
	foreach (int iLabel, labels)
	{
		QList<int> nums = datas.values(iLabel);
		emit logout(tr("loading"));
		foreach (int iNum, nums)
		{
			sprintf(inputPath, "F:/MD/flower/flowers/train82_fixed/segmim_%05d.jpg", iNum);
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
	LoadLabel();
	cv::FileStorage fsfesture("F:/MD/flower/flowers/bow_yml/bowfeature.xml", cv::FileStorage::READ);
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
	svm.save("F:/MD/flower/flowers/svm.xml");
}

void ImageFolderLoader::SVMTest()
{
	LoadLabel();
	qDebug() << datas;
	CvSVM svm;
	svm.load("F:/MD/flower/flowers/svm.xml");

	cv::Ptr<cv::DescriptorMatcher> matcher(new cv::FlannBasedMatcher);
	cv::Ptr<cv::FeatureDetector> detector(new cv::SiftFeatureDetector(200));
	cv::Ptr<cv::DescriptorExtractor> extractor(new cv::SiftDescriptorExtractor);

	cv::FileStorage fs("F:/MD/flower/flowers/bow_yml/bowkmean.xml", cv::FileStorage::READ);
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
		std::string str = file.toLocal8Bit().data();
		QString strTestFile = tr("F:/MD/flower/flowers/train/") + fileName;
		cv::Mat mat = cv::imread(strTestFile.toLocal8Bit().data());
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
		qDebug() << fResponse << "--" << iLabel << "--" << iNum;
// 		QString s = str.mid(7, 5);
// 		int iNum = s.toInt();
// 		int iLabel = vecLabels[iNum - 1];
	}

	if (iCount > 0)
	{
		emit logout(tr("%1").arg(iCorrect * 1.0/ iCount));
	}

}

void ImageFolderLoader::PrepareData()
{
	LoadLabel();
	QString strFolder = "F:/MD/flower/flowers/train82_10/";
	QString strFolderOut = "F:/MD/flower/flowers/validation/";
	QDir dir(strFolder);
	QStringList nameFilters;
	nameFilters << "*.bmp" << "*.jpg" << "*.png";
	QFileInfoList infoList = dir.entryInfoList(nameFilters, 
		QDir::Files | QDir::Readable);
	QDir dirSrc("");
	QFileInfoList infoListSrc = dirSrc.entryInfoList(nameFilters,
		QDir::Files | QDir::Readable);

	foreach(QFileInfo info, infoList)
	{
		
	}
	
	QMap<int, int> labelCount;
	
	foreach (QFileInfo infoSrc, infoListSrc)
	{
		if (strFileNames.contains(infoSrc.fileName()))
			continue;
		QString sFileName = infoSrc.fileName();
		QString s = sFileName.mid(7, 5);
		int iNum = s.toInt();
		int iLabel = datas.key(iNum);
		
	}

	foreach(QFileInfo info, infoList)
	{
		QString sFileName = info.fileName();
		QString s = sFileName.mid(7, 5);
		int iNum = s.toInt();
		int iLabel = datas.key(iNum);

		

		if (!labelCount.contains(iLabel))
			labelCount[iLabel] = 0;
		
		labelCount[iLabel]++;
		if (labelCount[iLabel] <= 20)
		{
			QString sOutputFile = strFolderOut + sFileName;
			QFile::copy(info.absoluteFilePath(), sOutputFile);
			QFile::remove(info.absoluteFilePath());
		}
	}
}

}//!< namespace InputLoader