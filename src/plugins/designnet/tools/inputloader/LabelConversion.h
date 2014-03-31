#pragma once
#include "..\..\designnet_core\designnetbase\processor.h"

namespace InputLoader{

class LabelConversion : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(LabelConversion)

	LabelConversion(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~LabelConversion();

	virtual QString title() const;
	virtual QString category() const;//!< 种类

	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
	virtual bool finishProcess();

	void setPath(const QString &p);
	QString path() const;

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s);

protected:

	QString m_filePath;
};
}