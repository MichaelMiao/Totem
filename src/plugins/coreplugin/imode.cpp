#include "imode.h"
using namespace Core;

IMode::IMode(QObject *parent) :
    IContext(parent),
    m_isEnabled(true)
{

}

bool IMode::isEnabled() const
{
    return m_isEnabled;
}

void IMode::setEnabled(bool enabled)
{
    if (m_isEnabled == enabled)
        return;
    m_isEnabled = enabled;
    emit enabledStateChanged(m_isEnabled);
}
