#ifndef MAXBOUNDRECT_H
#define MAXBOUNDRECT_H

#include "designnet/designnet_core/designnetbase/processor.h"
#include <QObject>

class MaxBoundRect : public DesignNet::Processor
{
	Q_OBJECT

public:

	DECLEAR_PROCESSOR(MaxBoundRect)

	MaxBoundRect(DesignNet::DesignNetSpace *space, QObject *parent = 0);
	~MaxBoundRect();

	virtual QString title() const;
	virtual QString category() const;//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);			//!< ������
};

#endif // MAXBOUNDRECT_H
