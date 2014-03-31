#pragma once
#include <QObject>
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"


class FeatureSaverPrivate;
class FeatureSaver : public DesignNet::Processor
{
public:
	FeatureSaver(DesignNet::DesignNetSpace *space, QObject* parent = 0);
	~FeatureSaver(void);

	virtual DesignNet::Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;	//!< 种类

	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< 处理函数
	virtual bool finishProcess();

	virtual void showConfig();			//!< 显示config

	void setFilePath(const QString &fileName); //!< 设置特征文件路径
	QString getFileName();

	bool isFileOpen();
protected:

protected:
	FeatureSaverPrivate* d;
};

