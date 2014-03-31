#include "glcmblock.h"
#include "../../../designnet/designnet_core/data/imagedata.h"
#include "../../../designnet/designnet_core/designnetbase/port.h"
#include "../../../designnet/designnet_core/property/optionproperty.h"


const char INPUT_GRAYIMAGE[]	= "gray image";
const char OUTPUT_GLCMFEATURE[] = "output glcm feature";
using namespace DesignNet;

namespace FeatureExtraction{

class GLCMBlockPrivate
{
public:

	explicit GLCMBlockPrivate(GLCMBlock *block) : q(block) { }
	~GLCMBlockPrivate() { }

	OptionProperty *m_typeOption;	//!< glcm的类型，有笛卡尔坐标系下的GLCM，有极坐标系下的GLCM
	GLCMBlock *q;
};

GLCMBlock::GLCMBlock(DesignNet::DesignNetSpace *space, QObject *parent )
	: Processor(space, parent),
	d(new GLCMBlockPrivate(this))
{
	d->m_typeOption = new OptionProperty(QLatin1String("type"), 
		tr("Type"), this);
	d->m_typeOption->addOption(tr("Normal GLCM"), GLCMType::Normal);
	d->m_typeOption->addOption(tr("Polor GLCM"), GLCMType::Polor);
	addProperty(d->m_typeOption);
	setName(tr("GLCM"));
	addPort(Port::IN_PORT, DATATYPE_GRAYIMAGE, INPUT_GRAYIMAGE);
}

GLCMBlock::~GLCMBlock()
{
	if(d)
	{
		delete d;
		d = 0;
	}
}

QString GLCMBlock::title() const
{
	return tr("GLCM");
}

QString GLCMBlock::category() const
{
	return tr("FeatureExtraction");
}

bool GLCMBlock::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	return true;
}

void GLCMBlock::propertyChanged( DesignNet::Property *prop )
{

}
}
