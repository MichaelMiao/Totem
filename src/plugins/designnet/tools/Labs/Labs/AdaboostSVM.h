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
	virtual QString category() const;//!< ����

	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ������
	virtual bool finishProcess();

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

	void prepareData();
	void LoadLabel(QMap<int, int> &imgLabel);

	void train();
	void test();
};
