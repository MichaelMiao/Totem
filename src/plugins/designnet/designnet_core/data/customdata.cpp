#include "customdata.h"
#include "designnetconstants.h"
#include "Utils/totemassert.h"

namespace DesignNet{

CustomData::CustomData(void* defaultValue,QObject *parent)
	: IData(parent), m_value(defaultValue)
{

}

CustomData::~CustomData()
{

}

IData* CustomData::clone( QObject *parent )
{
	return 0;
}

bool CustomData::copy( IData* data )
{
	TOTEM_ASSERT(data->id() == id(), return false);
	CustomData *customData = qobject_cast<CustomData*>(data);
	if(!customData)
		return false;

	return IData::copy(data);
}

bool CustomData::isValid() const
{
	return (bool)m_value;
}

QImage CustomData::image()
{
	return m_image;
}

void* CustomData::data() const
{
	return m_value;
}

}
