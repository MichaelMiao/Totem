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
	virtual QString category() const;//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);			//!< 处理函数

private:

	DesignNet::Port m_inputBinaryImagePort;	//!< 输入二值图端口
	DesignNet::Port m_outputRectPort;	//!< 输输出Rect端口
};

#endif // MAXBOUNDRECT_H
