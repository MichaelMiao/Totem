#pragma once


#include "coreplugin/imode.h"

namespace Core{
class IEditor;
class Context;
}

namespace DesignNet{

class DesignNetFrontWidget;
class DesignNetUserMode : public Core::IMode
{
    Q_OBJECT

public:

    explicit DesignNetUserMode(QObject *parent = 0);
    virtual ~DesignNetUserMode();
    static DesignNetUserMode *instance();

signals:

	void actionsUpdated(Core::IEditor *editor);

public slots:

    void currentEditorChanged(Core::IEditor *editor);
    void updateContext(Core::IMode *newMode, Core::IMode *oldMode);
    void updateActions();

private:
    
	void setActiveContext(const Core::Context &context);

	DesignNetFrontWidget*	m_pWidget;
};
}