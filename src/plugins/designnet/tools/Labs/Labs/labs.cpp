#include "stdafx.h"
#include "labs.h"
#include "AdaboostSVM.h"
#include <QtPlugin>

bool Labs::initialize(const QStringList &arguments, QString *errorString)
{
	return true;
}

void Labs::extensionsInitialized()
{
	m_pAdaboostSVM = new AdaboostSVMProcessor(0);

	addAutoReleasedObject(m_pAdaboostSVM);
}

Q_EXPORT_PLUGIN2(Labs, Labs)