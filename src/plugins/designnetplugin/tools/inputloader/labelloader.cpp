#include "labelloader.h"
using namespace DesignNet;
namespace InputLoader{
LabelLoader::LabelLoader(DesignNetSpace *space, QGraphicsItem *parent)
	: DesignNet::ProcessorGraphicsBlock(0, parent)
{

}

LabelLoader::~LabelLoader()
{

}

Processor* LabelLoader::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return 0;
}

QString LabelLoader::title() const
{
	return "";
}

QString LabelLoader::category() const
{
	return "";
}

bool LabelLoader::process()
{
	return true;
}

void LabelLoader::dataArrived( DesignNet::Port* port )
{
	ProcessorGraphicsBlock::dataArrived(port);
}

void LabelLoader::propertyChanged( DesignNet::Property *prop )
{

}

}
