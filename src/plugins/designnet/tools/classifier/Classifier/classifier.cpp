#include "stdafx.h"
#include "classifier.h"
#include <QtPlugin>
#include "SVMClassifier.h"


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
	addAutoReleasedObject(new SVMClassifer(0));
}
Q_EXPORT_PLUGIN2(Classifier, Classifier)