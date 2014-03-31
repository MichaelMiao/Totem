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
	virtual QString category() const;	//!< ����

	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ������
	virtual bool finishProcess();

	virtual void showConfig();			//!< ��ʾconfig

	void setFilePath(const QString &fileName); //!< ���������ļ�·��
	QString getFileName();

	bool isFileOpen();
protected:

protected:
	FeatureSaverPrivate* d;
};

