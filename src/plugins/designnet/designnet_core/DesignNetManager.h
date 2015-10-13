#pragma once
#include "designnetmainwindow.h"


class DesignNetManager : public QObject
{
	Q_OBJECT

public:

	
	static DesignNetManager* instance()
	{
		static DesignNetManager mgr;
		return &mgr;
	}
	~DesignNetManager();

	void init();
	Core::IEditor* createEditor(QWidget* pParent);

private:

	DesignNetManager();

	DesignNetMainWindow*	m_pMainWindow;
};
