#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "operations_global.h"
#include "extensionsystem/iplugin.h"
class JoinProcessor;
class BufferProcessor;
class FeatureSaver;
class OPERATIONS_EXPORT Operations : public ExtensionSystem::IPlugin
{

public:

	Operations();
	~Operations();
	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();

protected:

	JoinProcessor *		m_join;
	BufferProcessor*	m_buffer;
	FeatureSaver*		m_featureSaver;
};

#endif // OPERATIONS_H
