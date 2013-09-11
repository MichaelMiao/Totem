#include "maxboundrect.h"
#include "designnet/designnet_core/data/customdata.h"

using namespace DesignNet;

MaxBoundRect::MaxBoundRect(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent),
	m_outputRectPort(new CustomData(0, "Rect", this), Port::OUT_PORT, tr("Out Rect"), this),
	m_inputBinaryImagePort(new CustomData(0, "Rect", this), Port::OUT_PORT, tr("Out Rect"), this)
{
	addPort(&m_inputBinaryImagePort);
	addPort(&m_outputRectPort);
}

MaxBoundRect::~MaxBoundRect()
{

}

Processor* MaxBoundRect::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return 0;
}

QString MaxBoundRect::title() const
{
	return tr("Max bound rect");
}

QString MaxBoundRect::category() const
{
	return tr("");
}

bool MaxBoundRect::process( QFutureInterface<DesignNet::ProcessResult> &future )
{
	return true;
}
