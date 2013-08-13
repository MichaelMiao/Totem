#include "designnetcontext.h"
#include "coreplugin/icontext.h"
#include "designnetconstants.h"

#include <QWidget>

namespace DesignNet{

DesignNetContext::DesignNetContext(const Core::Context &contexts, QWidget *widget,
                                   QObject *parent)
    : Core::IContext(parent)
{
    setContext(contexts);
    setWidget(widget);
}
}
