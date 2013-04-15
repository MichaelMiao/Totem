#include "totem_gui_pch.h"
#include "labeldata.h"
#include "designnetconstants.h"
namespace DesignNet{

LabelData::LabelData(QObject *parent)
	: IData(parent)
{

}
LabelData::~LabelData()
{

}

Core::Id DesignNet::LabelData::id()
{
	return DesignNet::Constants::DATA_TYPE_LABEL;
}

}

QHash<int, int> & DesignNet::LabelData::labels()
{
	return m_labels;
}
