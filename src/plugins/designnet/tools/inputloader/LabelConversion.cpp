#include "stdafx.h"
#include "LabelConversion.h"
#include "..\..\designnet_core\designnetbase\port.h"

static const char INPORT_LABELMATRIX[] = "Label Matrix";

using namespace DesignNet;
namespace InputLoader
{
LabelConversion::LabelConversion(DesignNet::DesignNetSpace *space, QObject *parent /*= 0*/)
	: Processor(space, parent)
{
	addPort(Port::IN_PORT, DATATYPE_MATRIX, INPORT_LABELMATRIX);
}

LabelConversion::~LabelConversion()
{

}

QString LabelConversion::title() const
{
	return tr("Label Conversion");
}

QString LabelConversion::category() const
{
	return tr("Loader");
}

bool LabelConversion::prepareProcess()
{

	return true;
}

bool LabelConversion::process(QFutureInterface<DesignNet::ProcessResult> &future)
{
	return true;
}

bool LabelConversion::finishProcess()
{
	return true;
}

void LabelConversion::serialize(Utils::XmlSerializer& s) const
{

}

void LabelConversion::deserialize(Utils::XmlDeserializer& s)
{

}

}