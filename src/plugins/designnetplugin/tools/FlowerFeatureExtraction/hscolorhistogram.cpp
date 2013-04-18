#include "hscolorhistogram.h"
#include "designnetplugin/designnet_core/graphicsitem/portgraphicsitem.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include <opencv2/imgproc/imgproc.hpp>
using namespace DesignNet;
namespace FlowerFeatureExtraction{

HSColorHistogram::HSColorHistogram(DesignNet::DesignNetSpace *space, QGraphicsItem *parent)
	: ProcessorGraphicsBlock(space, parent),
	m_outputPort(new ImageData(this), Port::OUT_PORT),
	m_inputPort(new ImageData(this),Port::IN_PORT)
{

}

HSColorHistogram::~HSColorHistogram()
{

}

Processor* HSColorHistogram::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{

}

QT_NAMESPACE::QString HSColorHistogram::title() const
{

}

QT_NAMESPACE::QString HSColorHistogram::category() const
{

}

bool HSColorHistogram::process()
{

}

void HSColorHistogram::dataArrived( DesignNet::Port* port )
{

}

void HSColorHistogram::propertyChanged( DesignNet::Property *prop )
{

}

}
