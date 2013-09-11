#include "customdata.h"
#include "designnetconstants.h"
#include "Utils/totemassert.h"

namespace DesignNet{

CustomData::CustomData(void* defaultValue, const QString &strLabel, QObject *parent)
	: IData(parent), m_value(defaultValue), m_label(strLabel)
{

}

CustomData::~CustomData()
{

}

Core::Id CustomData::id()
{
	return Constants::DATA_TYPE_CUSTOM;
}

IData* CustomData::clone( QObject *parent )
{
//	CustomData *customData = new CustomData(this->data(), parent);
//	return customData;
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
	m_image.load(Constants::DATA_IMAGE_INT);
	return m_image;
}

void* CustomData::data( const QString &strLabel ) const
{
	if(strLabel != m_label)
		return 0;

	return m_value;
}

}
