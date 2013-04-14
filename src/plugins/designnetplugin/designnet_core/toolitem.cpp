#include "toolitem.h"
#include "ProcessorGraphicsBlock.h"
namespace DesignNet{
ToolItem::ToolItem(ProcessorGraphicsBlock* data)
    : m_data(data)
{
    m_parent = 0;
}

ToolItem::~ToolItem()
{
    qDeleteAll(m_children);
}

ToolItem *ToolItem::parent() const
{
    return m_parent;
}

void ToolItem::setParent(ToolItem *parent)
{
    m_parent = parent;
}

ToolItem *ToolItem::childAt(int row) const
{
    return m_children.value(row);
}

QList<ToolItem *> ToolItem::children() const
{
    return m_children;
}


bool ToolItem::hasChildren()
{
    return !m_children.isEmpty();
}

int ToolItem::childCount()
{
    return m_children.size();
}

int ToolItem::rowOfChild(ToolItem *item)
{
    return m_children.indexOf(item);
}

ProcessorGraphicsBlock *ToolItem::data() const
{
    return m_data;
}

QString ToolItem::text() const
{
    return m_text;
}

void ToolItem::setText(QString text)
{
    m_text = text;
}

void ToolItem::addChild(ToolItem *child)
{
    child->setParent(this);
    m_children.append(child);
}

}
