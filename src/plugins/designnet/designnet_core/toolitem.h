#ifndef TOOLITEM_H
#define TOOLITEM_H

#include <QList>
#include <QString>
namespace DesignNet{

class Processor;
class ToolItem
{
public:
    ToolItem(Processor* data = 0);
    virtual ~ToolItem();

    ToolItem* parent() const;    
    void setParent(ToolItem* parent);

    ToolItem* childAt(int row) const;
    QList<ToolItem*> children() const;
    void    insertChild(int row, ToolItem* child);
    void    addChild(ToolItem* child);
    bool    hasChildren();
    int     childCount();
    int     rowOfChild(ToolItem* item);
    Processor*     data() const;
    QString text() const;
    void setText(const QString &t);
protected:
    QList<ToolItem*>        m_children; //!< 所有孩子
    ToolItem*               m_parent;   //!< 父结点
    Processor*				m_data;     //!< Tool数据
    QString                 m_text;     //!< 非Tool结点显示类别
};
}

#endif // TOOLITEM_H
