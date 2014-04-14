#include "DesignNetUserMode.h"
#include "designnetfrontwidget.h"


namespace DesignNet
{
static DesignNetUserMode *m_instance = 0;

DesignNetUserMode::DesignNetUserMode(QObject *parent)
	: Core::IMode(parent)
{
	m_instance = this;
	setObjectName(QLatin1String("DesignNetUserMode"));
	setDisplayName(tr("UserMode"));
	setIcon(QIcon(QLatin1String(":/core/images/mode_edit.png")));
	//设置模式优先级
	setPriority(10);
	setId("DesignNetUserMode");
	setType("DesignNetUserMode");

	//////////////////////////////////////////////////////////////////////////

	setWidget(new QWidget);
}

DesignNetUserMode::~DesignNetUserMode()
{

}

void DesignNetUserMode::currentEditorChanged(Core::IEditor *editor)
{

}

void DesignNetUserMode::updateContext(Core::IMode *newMode, Core::IMode *oldMode)
{

}

void DesignNetUserMode::updateActions()
{

}

DesignNetUserMode * DesignNet::DesignNetUserMode::instance()
{
	return m_instance;
}

}
