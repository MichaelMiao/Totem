#include "stdafx.h"
#include "Processor.h"


bool DesignNet::Processor::connectTo(Processor* child)
{
	return true;
}

bool DesignNet::Processor::isConnectTo(Processor* pChild)
{
	return true;
}

void DesignNet::Processor::detach(Processor* pChild)
{
}

void DesignNet::Processor::detach()
{

}

bool DesignNet::Processor::connectionTest(Port* pOutput, Port* pInput)
{
	return true;
}

bool DesignNet::Processor::connectionTest(Processor* father)
{
	return true;
}
