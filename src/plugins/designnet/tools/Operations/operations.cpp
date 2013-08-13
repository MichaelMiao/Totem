#include "operations.h"
#include "joinprocessor.h"
#include "bufferprocessor.h"
#include <QtPlugin>
Operations::Operations()
{
	m_join = 0;
	m_buffer= 0;
}

Operations::~Operations()
{

}

bool Operations::initialize( const QStringList &arguments, QString *errorString )
{
	return true;
}

void Operations::extensionsInitialized()
{
	m_join = new JoinProcessor(0);
	m_buffer = new BufferProcessor(0);
	addAutoReleasedObject(m_join);
	addAutoReleasedObject(m_buffer);
}
Q_EXPORT_PLUGIN2(Operations, Operations)