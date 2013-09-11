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
 * ���ڽ�������������һ��m������Ϊ1*n������������Щ����������һ��
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
	virtual QString category() const;	//!< ����
	virtual bool process(QFutureInterface<DesignNet::ProcessResult> &future);				//!< ������
	virtual void serialize(Utils::XmlSerializer& s) const;
	virtual void deserialize(Utils::XmlDeserializer& s) ;

signals:
protected:
	virtual void propertyChanged(DesignNet::Property *prop);
private:
	JoinProcessorPrivate *d;
};

#endif // JOIN_H
