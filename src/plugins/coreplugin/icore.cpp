#include "stdafx.h"
#include "icore.h"


void ICore::initialize()
{
	createMainWindow();
}

void ICore::extensionsInitialized()
{

}

MainWindow* ICore::createMainWindow()
{
	MainWindow* pNewWindow = new MainWindow();
	pNewWindow->readSettings();
	pNewWindow->show();
	return pNewWindow;
}
