#pragma once
#include "../property/propertyowner.h"
#include "port.h"


namespace DesignNet
{
class Property;
class Processor : public PropertyOwner
{
public:

	QList<Port*> getPorts(Port::PortType pt) const;

	bool connectTo(Processor* pProcessor);


signals:

private:

	QIcon			m_icon;
	QString			m_title;				//!< 种类title
	QList<Port*>	m_outputPort;			//!< 所有的输出端口
	QList<Port*>	m_inputPort;			//!< 输入端口
};
}
