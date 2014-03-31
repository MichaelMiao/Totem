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
	virtual QString category() const;//!< ����

	virtual bool prepareProcess();
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);     //!< ��������
	virtual bool finishProcess();

	void setPath(const QString &p);
	QString path() const;
	
private:
	
	virtual void propertyChanged(DesignNet::Property *prop);

	QMap<int, int> m_mapLabel;
};
}