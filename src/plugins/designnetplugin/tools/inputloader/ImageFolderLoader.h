#pragma once
#include "designnetplugin\designnet_core\graphicsitem\processorgraphicsblock.h"
#include "designnetplugin\designnet_core\designnetbase\port.h"
namespace DesignNet{
class PathDialogProperty;
}
namespace InputLoader{

class ImageFolderLoader :
	public DesignNet::ProcessorGraphicsBlock
{
	Q_OBJECT
public:
	ImageFolderLoader(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
	virtual ~ImageFolderLoader(void);
	virtual Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process();     //!< 处理函数
signals:
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
	DesignNet::Port m_outPort;
	DesignNet::PathDialogProperty *m_pathProperty;
};

}

