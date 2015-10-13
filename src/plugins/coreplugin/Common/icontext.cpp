#include "stdafx.h"
#include "icontext.h"
#include "id.h"

namespace Core{

Context::Context(const char *id, int offset)
{
    d.append(Id(QLatin1String(id) + QString::number(offset)).uniqueIdentifier());
}

void Context::add(const char *id)
{
    d.append(Id(id).uniqueIdentifier());
}

bool Context::contains(const char *id) const
{
    return d.contains(Id(id).uniqueIdentifier());
}
}
