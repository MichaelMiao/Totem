#include "preprocessor.h"
#include <QtPlugin>
#include "GrabCutProcessor.h"

Preprocessor::Preprocessor()
{

}

Preprocessor::~Preprocessor()
{

}

bool Preprocessor::initialize(const QStringList &arguments, QString *errorString)
{
	return true;
}

void Preprocessor::extensionsInitialized()
{
	addAutoReleasedObject(new GrabCutProcessor(0));
}

Q_EXPORT_PLUGIN2(Preprocessor, Preprocessor)
