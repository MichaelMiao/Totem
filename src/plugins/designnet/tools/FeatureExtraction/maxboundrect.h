#ifndef MAXBOUNDRECT_H
#define MAXBOUNDRECT_H

#include "designnet/designnet_core/designnetbase/processor.h"
#include <QObject>

class MaxBoundRect : public DesignNet::Processor
{
	Q_OBJECT

public:
	MaxBoundRect(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~MaxBoundRect();

	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);			//!< ������

private:

	DesignNet::Port m_inputBinaryImagePort;	//!< �����ֵͼ�˿�
	DesignNet::Port m_outputRectPort;	//!< �����Rect�˿�
};

#endif // MAXBOUNDRECT_H
