#include "stdafx.h"
#include "Processor.h"


QList<DesignNet::Port*> DesignNet::Processor::getPorts(Port::PortType pt) const
{
	if (pt == DesignNet::Port::IN_PORT)
		return m_inputPort;
	else
		return m_outputPort;
}

bool DesignNet::Processor::connectTo(Processor* pProcessor)
{
	bool bValid = false;
	QList<DesignNet::Port*> ports = pProcessor->getPorts(DesignNet::Port::IN_PORT);
	for (QList<DesignNet::Port*>::iterator itr = m_outputPort.begin(); itr != m_outputPort.end(); itr++)
	{
		for (QList<DesignNet::Port*>::iterator itrIn = m_outputPort.begin(); itrIn != m_outputPort.end(); itrIn++)
		{
			bValid = bValid || (*itr)->connectTo(*itrIn);
		}
	}
	return bValid;
}

bool DesignNet::Processor::canConnectTo(Processor* pProcessor)
{
	return true;
}
