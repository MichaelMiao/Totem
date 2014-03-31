#include "maxboundrect.h"
#include "designnet/designnet_core/data/customdata.h"

using namespace DesignNet;

MaxBoundRect::MaxBoundRect(DesignNet::DesignNetSpace *space, QObject *parent)
	: Processor(space, parent)
{
	addPort(Port::IN_PORT, DATATYPE_USERTYPE, tr("Input Rect"));
	addPort(Port::OUT_PORT, DATATYPE_USERTYPE, tr("Out Rect"));
	setName(tr("MaxBoundRect"));
}

MaxBoundRect::~MaxBoundRect()
{

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
