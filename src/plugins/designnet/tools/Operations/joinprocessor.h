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
 * ���ڽ�������������һ��m������Ϊ1*n������������Щ����������һ��
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
	virtual QString category() const;	//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);				//!< ������
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

signals:
protected:
	virtual void propertyChanged(DesignNet::Property *prop);

};

#endif // JOIN_H
