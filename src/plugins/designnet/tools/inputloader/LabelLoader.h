#pragma once
#include "..\..\designnet_core\designnetbase\processor.h"

namespace InputLoader{

class LabelLoader : public DesignNet::Processor
{
	Q_OBJECT

public:

	LabelLoader(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~LabelLoader(void);
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类

	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
	virtual bool finishProcess();

	void setPath(const QString &p);
	QString path() const;

	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s);
};
}