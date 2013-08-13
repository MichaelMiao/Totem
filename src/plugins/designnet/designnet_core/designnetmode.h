#ifndef DESIGNMODE_H
#define DESIGNMODE_H

#include "coreplugin/imode.h"

namespace Core{
class IEditor;
class Context;
}

namespace DesignNet{
class DesignNetModePrivate;

/**
 * \class	DesignNetMode
 *
 * \brief	DesignNet mode.
 *
 * \author	Michael_BJFU
 * \date	2013/6/3
 */

class DesignNetMode : public Core::IMode
{
    Q_OBJECT
public:
    explicit DesignNetMode(QObject *parent = 0);
    virtual ~DesignNetMode();
    static DesignNetMode *instance();

    void registerDesignWidget(QWidget *widget,
                              const QStringList &suffixTypes,
                              const Core::Context &context);
    void unregisterDesignWidget(QWidget *widget);

    QStringList registeredMimeTypes() const;

signals:
    void actionsUpdated(Core::IEditor *editor);

public slots:
    void currentEditorChanged(Core::IEditor *editor);
    void updateContext(Core::IMode *newMode, Core::IMode *oldMode);
    void updateActions();

private:
    void setActiveContext(const Core::Context &context);

    DesignNetModePrivate *d;
};
}

#endif // DESIGNMODE_H
