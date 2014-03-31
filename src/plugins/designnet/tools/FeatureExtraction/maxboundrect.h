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
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);			//!< 处理函数
};

#endif // MAXBOUNDRECT_H
