#pragma once
#include "../actionmanager/command.h"
#include "../Common/id.h"


class CORE_EXPORT RebarManager : public QObject
{
	Q_OBJECT
public:

	static RebarManager* instance() 
	{
		static RebarManager mgr;
		return &mgr;
	}

	bool	addRebarCommand(Core::Id idCategory, Core::Command* pCommand);
	int		getCategoryCount() { return m_mapRebarAction.count(); }
	QVector<Core::Id>& getActions(Core::Id idCategory);

signals:

	void	rebarObjectAdded(const Core::Id& idCategory, Core::Command* pCommand);

private:

	RebarManager();
	~RebarManager() {}

	QMap<int, QSet<int> > m_mapRebarAction;
};
