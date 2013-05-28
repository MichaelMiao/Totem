#include "glcmblock.h"
#include "designnetplugin/designnet_core/designnetbase/port.h"
#include "designnetplugin/designnet_core/data/imagedata.h"
#include "designnetplugin/designnet_core/property/optionproperty.h"
using namespace DesignNet;
namespace FeatureExtraction{
class GLCMBlockPrivate{
public:
	GLCMBlockPrivate(GLCMBlock *block)
		: q(block),m_inputPort(new ImageData(ImageData::IMAGE_GRAY), Port::IN_PORT)
	{
	}
	~GLCMBlockPrivate(){
	}
	Port			m_inputPort;
	OptionProperty *m_typeOption;	//!< glcm的类型，有笛卡尔坐标系下的GLCM，有极坐标系下的GLCM

	GLCMBlock *q;
};
GLCMBlock::GLCMBlock(DesignNet::DesignNetSpace *space, QGraphicsItem *parent )
	: ProcessorGraphicsBlock(space, parent),
	d(new GLCMBlockPrivate(this))
{
	d->m_typeOption = new OptionProperty(QLatin1String("type"), 
		tr("Type"), this);
	d->m_typeOption->addOption(tr("Normal GLCM"), GLCMType::Normal);
	d->m_typeOption->addOption(tr("Polor GLCM"), GLCMType::Polor);
	addProperty(d->m_typeOption);
	setName(tr("GLCM"));
}

GLCMBlock::~GLCMBlock()
{
	if(d)
	{
		delete d;
		d = 0;
	}
}

Processor* GLCMBlock::create( DesignNet::DesignNetSpace *space /*= 0*/ ) const
{
	return new GLCMBlock(space);
}

QString GLCMBlock::title() const
{
	return tr("GLCM");
}

QString GLCMBlock::category() const
{
	return tr("FeatureExtraction");
}

bool GLCMBlock::process()
{
	return true;
}

void GLCMBlock::dataArrived( DesignNet::Port* port )
{

}

void GLCMBlock::propertyChanged( DesignNet::Property *prop )
{

}

bool GLCMBlock::connectionTest( DesignNet::Port* src, DesignNet::Port* target )
{
	if(target == &d->m_inputPort)
	{
		ImageData *srcData = qobject_cast<ImageData*>(src->data());
		if (!srcData || srcData->imageType() != ImageData::IMAGE_GRAY)
		{
			return false;
		}
		return true;
	}
	return false;
}

}