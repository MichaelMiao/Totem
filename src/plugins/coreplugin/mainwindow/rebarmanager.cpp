#include "stdafx.h"
#include "rebarmanager.h"


RebarManager::RebarManager()
{
	
}

bool RebarManager::addRebarCommand(Core::Id idCategory, Core::Command* pCommand)
{
	int iId = idCategory.uniqueIdentifier();
	if (m_mapRebarAction[iId].contains(pCommand->id().uniqueIdentifier()))
		return false;
	m_mapRebarAction[iId].insert(pCommand->id().uniqueIdentifier());
	emit rebarObjectAdded(idCategory, pCommand);
	return true;
}
