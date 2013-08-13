#ifndef DESIGNNETCONTEXT_H
#define DESIGNNETCONTEXT_H
#include "coreplugin/icontext.h"
#include "designnet_core_global.h"

namespace DesignNet{

class DesignNetContext : public Core::IContext
{
public:
    DesignNetContext(const Core::Context &contexts,QWidget *widget,
                     QObject *parent = 0);

};
}

#endif // DESIGNNETCONTEXT_H
