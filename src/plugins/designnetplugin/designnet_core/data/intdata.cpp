#include "intdata.h"
#include "designnetconstants.h"
#include "Utils/totemassert.h"
namespace DesignNet{

IntData::IntData(const int &defaultValue, QObject *parent)
	: IData(parent), m_value(defaultValue)
{
	m_image.load(Constants::DATA_IMAGE_INT);
}

IntData::~IntData()
{

}

void IntData::setValue( const int &value )
{
	m_value = value;
}

int IntData::value() const
{
	return m_value;
}

Core::Id IntData::id()
{
	return Constants::DATA_TYPE_INT;
}

IData* IntData::clone( QObject *parent /*= 0*/ )
{
	IntData *intData = new IntData(this->value(), parent);
	return intData;
}

bool IntData::copy( IData* data )
{
	TOTEM_ASSERT(data->id() == id(), return false);
	IntData *intData = qobject_cast<IntData*>(data);
	if(!intData)
	{
		return false;
	}
	m_value = intData->value();
	emit dataChanged();
	return true;
}

bool IntData::isValid() const
{
	return true;
}

QImage IntData::image()
{
	return m_image;
}

}
