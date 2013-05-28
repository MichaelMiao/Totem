#ifndef JOIN_H
#define JOIN_H

#include <QObject>
#include "designnetplugin\designnet_core\graphicsitem\processorgraphicsblock.h"
#include "designnetplugin\designnet_core\designnetbase\port.h"
QT_BEGIN_NAMESPACE
class QMenu;
QT_END_NAMESPACE
namespace DesignNet{
class DesignNetSpace;
}
class JoinProcessorPrivate;
/**
 * \class	Join
 *
 * \brief	Join.
 * 
 * 用于将处理结果连接在一起，m个输入为1*n的向量，将这些向量连接在一起
 * \author	Michael_BJFU
 * \date	2013/5/22
 */

class JoinProcessor : public DesignNet::ProcessorGraphicsBlock
{
	Q_OBJECT

public:
	JoinProcessor(DesignNet::DesignNetSpace *space, QGraphicsItem *parent = 0);
	virtual ~JoinProcessor();
	virtual DesignNet::Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;//!< 种类
	virtual bool process();     //!< 处理函数
signals:
protected:
	virtual void dataArrived(DesignNet::Port* port);  //!< 数据到达
	virtual void createContextMenu(QMenu *parentMenu);
	virtual void propertyChanged(DesignNet::Property *prop);
private:
	JoinProcessorPrivate *d;
};

#endif // JOIN_H
