#include "testlifan.h"
#include "designnetplugin/designnet_core/data/matrixdata.h"
using namespace DesignNet;
TestLiFan::TestLiFan(DesignNet::DesignNetSpace *space, QGraphicsItem *parent )
	: ProcessorGraphicsBlock(space, parent),
	m_inputPort(new DesignNet::MatrixData(this), Port::IN_PORT, "GrayImage")
{
	addPort(&m_inputPort);
}

TestLiFan::~TestLiFan()
{

}

Processor* TestLiFan::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new TestLiFan(space);
}

QString TestLiFan::title() const
{
	return tr("LiFan");
}

QString TestLiFan::category() const
{

		return tr("LiFan/LiTest");
}

bool TestLiFan::process()
{
	IData* m = m_inputPort.data();
	pushData(m,"")
	return true;
}

void TestLiFan::dataArrived( DesignNet::Port* port )
{

}

void TestLiFan::propertyChanged( DesignNet::Property *prop )
{

}

bool TestLiFan::connectionTest( DesignNet::Port* src, DesignNet::Port* target )
{
	return true;
}
