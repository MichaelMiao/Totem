#pragma once
#include "../property/propertyowner.h"
#include "port.h"
#include "coreplugin/Common/id.h"


namespace DesignNet
{
class Property;
class Port;

class Processor : public QObject, public PropertyOwner
{
	Q_OBJECT

public:

	QList<Port*> getPorts(Port::PortType pt) const;

	bool connectTo(Processor* pProcessor);
	bool canConnectTo(Processor* pProcessor);
	
	// override

	virtual bool testConnect(Port* port) = 0;

signals:

private:

	QIcon			m_icon;
	QString			m_title;				//!< 种类title
	QList<Port*>	m_outputPort;			//!< 所有的输出端口
	QList<Port*>	m_inputPort;			//!< 输入端口
};
}
