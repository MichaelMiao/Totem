#pragma once
#include "designnet\designnet_core\designnetbase\processor.h"
#include "designnet\designnet_core\data\imagedata.h"



class AdaboostSVMProcessor : public DesignNet::Processor
{
	Q_OBJECT
public:
	
	DECLEAR_PROCESSOR(AdaboostSVMProcessor)
	
	AdaboostSVMProcessor(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	virtual ~AdaboostSVMProcessor(void);
	virtual QString title() const { return tr("AdaboostSVM"); }
	virtual QString category() const;//!< 种类

	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
	virtual bool finishProcess();

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

	void prepareData();
	void createBowSiftData();

	void LoadLabel(QMap<int, int> &imgLabel);

	void train();
	void test();

private:

	cv::Mat m_colorTrain;
	cv::Mat m_shapeTrain;
	cv::Mat m_textureTrain;
	cv::Mat m_train;
	QList<QString> m_vecFiles;
	QMap<int, int> m_imgLabel;
	cv::Mat m_labelMat;
};
