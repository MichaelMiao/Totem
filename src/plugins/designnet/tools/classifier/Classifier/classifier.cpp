#include "stdafx.h"
#include "classifier.h"
#include <QtPlugin>


Classifier::Classifier()
{

}

Classifier::~Classifier()
{

}

bool Classifier::initialize(const QStringList &arguments, QString *errorString)
{
	return true;
}

void Classifier::extensionsInitialized()
{

}
Q_EXPORT_PLUGIN2(Classifier, Classifier)