#include "idata.h"

namespace DesignNet{

IData::IData(QObject *parent) :
    QObject(parent)
{
	m_index = -1;
	m_bPermanent = false;
}

void IData::setPermanent( const bool &p )
{
	m_bPermanent = p;
}

bool IData::isPermanent() const
{
	return m_bPermanent;
}

bool IData::copy( IData* data )
{
	m_index = data->index();
	emit dataChanged();
	return true;
}

}
