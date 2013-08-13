#ifndef JOIN_H
#define JOIN_H

#include <QObject>
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
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

class JoinProcessor : public DesignNet::Processor
{
	Q_OBJECT

public:
	JoinProcessor(DesignNet::DesignNetSpace *space, QObject* parent = 0);
	virtual ~JoinProcessor();
	virtual DesignNet::Processor* create(DesignNet::DesignNetSpace *space = 0) const;
	virtual QString title() const;
	virtual QString category() const;	//!< 种类
	virtual bool process();				//!< 处理函数
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

signals:
protected:
	virtual void propertyChanged(DesignNet::Property *prop);
private:
	JoinProcessorPrivate *d;
};

#endif // JOIN_H
