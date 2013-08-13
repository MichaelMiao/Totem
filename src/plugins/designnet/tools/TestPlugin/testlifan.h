#ifndef TESTLIFAN_H
#define TESTLIFAN_H

#include <QObject>
#include "designnetplugin/designnet_core/graphicsitem/processorgraphicsblock.h"
#include "designnetplugin/designnet_core/designnetbase/port.h"
#include "designnetplugin/designnet_core/data/imagedata.h"

class TestLiFan : public DesignNet::ProcessorGraphicsBlock
{
	Q_OBJECT

public:
	TestLiFan(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
	~TestLiFan();
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process();     //!< 处理函数
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
	virtual void propertyChanged(DesignNet::Property *prop);
	virtual bool connectionTest(DesignNet::Port* src, DesignNet::Port* target);
	DesignNet::Port m_inputPort;
};

#endif // TESTLIFAN_H
