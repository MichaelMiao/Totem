#pragma once
#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include "preprocessor_global.h"
#include "extensionsystem/iplugin.h"


class PREPROCESSOR_EXPORT Preprocessor : public ExtensionSystem::IPlugin
{
	Q_OBJECT

public:

	Preprocessor();
	~Preprocessor();

	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();

private:

};

#endif // PREPROCESSOR_H
