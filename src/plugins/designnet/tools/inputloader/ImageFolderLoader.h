#pragma once
#include "designnet\designnet_core\designnetbase\processor.h"
#include "designnet\designnet_core\data\imagedata.h"
#include <QReadWriteLock>


namespace DesignNet{
class PathDialogProperty;
}
namespace InputLoader{

class ImageFolderLoader : public DesignNet::Processor
{
	Q_OBJECT
public:
	ImageFolderLoader(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	virtual ~ImageFolderLoader(void);
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< ����
	
	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ������
	virtual bool finishProcess();

	void setPath(const QString &p);
	QString path() const;

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

signals:
protected:
	
	void LoadSiftFeature();
	void LoadLabel();
	void PrepareData();

	void LoadData();
	void SaveBinary();
	void SaveContours();
	void SaveFinal();
	
	void SiftMat(cv::Mat &mat, cv::Mat& mask, cv::Mat &descriptor);
	int MaskMat(cv::Mat &mat, cv::Mat& mask);

	void BOWCluster();

	void RepresentBOW();//!< ���ֵ��ʾ
	void SVMTrain();
	void SVMTest();
	void SVM_C();
	void SVM_SIFT();
	void SVM_HOG();


	QString			m_folderPath;
	QStringList		m_filePaths;
	int				m_iCurIndex;
	DesignNet::ImageData		m_imageData;

	mutable QReadWriteLock	m_lock;
};

}

