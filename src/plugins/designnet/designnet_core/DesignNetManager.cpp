#include "stdafx.h"
#include "DesignNetManager.h"
#include "DesignNetEditor.h"


DesignNetManager::DesignNetManager() : m_pMainWindow(0)
{

}

DesignNetManager::~DesignNetManager()
{

}

void DesignNetManager::init()
{
	m_pMainWindow = new DesignNetMainWindow(0);
	m_pMainWindow->initialize();
}

Core::IEditor* DesignNetManager::createEditor(QWidget* parent)
{
	DesignNetEditor *pEditor = new DesignNetEditor(parent, m_pMainWindow);
	m_pMainWindow->addEditor(pEditor);
	return pEditor;
}
