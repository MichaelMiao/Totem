#ifndef JOIN_H
#define JOIN_H

#include <QObject>
#include "designnet/designnet_core/designnetbase/processor.h"
#include "designnet/designnet_core/designnetbase/port.h"
#include "opencv2/core/core.hpp"

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

	DECLEAR_PROCESSOR(JoinProcessor)
	JoinProcessor(DesignNet::DesignNetSpace *space, QObject* parent = 0);
	virtual ~JoinProcessor();
	virtual QString title() const;
	virtual QString category() const;	//!< 种类
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);				//!< 处理函数
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

signals:
protected:
	virtual void propertyChanged(DesignNet::Property *prop);

};

#endif // JOIN_H
