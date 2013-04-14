#include "toolmodel.h"
#include "toolitem.h"
#include "processorgraphicsblock.h"
#include "extensionsystem/pluginmanager.h"
#include "processorfactory.h"
namespace DesignNet{
ToolModel* ToolModel::m_instance = 0;
ToolModel::ToolModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_root = new ToolItem;
    connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*)),
            this, SLOT(objectAdded(QObject*)));
    m_instance = this;
}

ToolModel::~ToolModel()
{
    if(m_root)
    {
        delete m_root;
        m_root = 0;
    }
}

ToolModel *ToolModel::instance()
{
    return m_instance;
}

int ToolModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant ToolModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.column() < 0 || index.column() > 2)
        return QVariant();
    ToolItem* parentItem = static_cast<ToolItem*>(index.internalPointer());
    ProcessorGraphicsBlock *pBlock = parentItem->data();
    if(!pBlock)
    {
        if(role == Qt::DisplayRole)
           return parentItem->text();
        return QVariant();
    }
    else
    {
        if(role == Qt::DisplayRole)
        {
            return pBlock->name();
        }
        if(role == Qt::DecorationRole)
        {
            return pBlock->getIcon();
        }
        if(role == Qt::UserRole)
            return qVariantFromValue((void*)pBlock);
        return QVariant();
    }
}

QVariant ToolModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole)
    {
        if(section == 0)
            return tr("Processors");
    }
	return QVariant();
}

QModelIndex ToolModel::index(int row, int column, const QModelIndex &parent) const
{
    if((row < 0 || column < 0))
        return QModelIndex();
    ToolItem* parentItem = itemForIndex(parent);
    if(ToolItem* item = parentItem->childAt(row))
    {
        return createIndex(row, column, item);
    }
    return QModelIndex();
}

QModelIndex ToolModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();
    if(ToolItem* childItem = itemForIndex(index))
    {
        if(ToolItem* parentItem = childItem->parent())
        {
            if(parentItem == m_root)
            {
                return QModelIndex();
            }
            if(ToolItem *grandItem = parentItem->parent())
            {
                int row = grandItem->rowOfChild(parentItem);
                return createIndex(row, 0, (void*)(parentItem));
            }
        }
    }
    return QModelIndex();
}

int ToolModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid() && parent.column() != 0)
    {
        return 0;
    }
    ToolItem *item = itemForIndex(parent);
    return item ? item->childCount() : 0;
}

Qt::ItemFlags ToolModel::flags(const QModelIndex &index) const
{
    if(index.isValid() && rowCount(index) == 0)
    {
        ToolItem *item = itemForIndex(index);
        if(item->data())
        {
            return Qt::ItemIsDragEnabled | Qt::ItemIsSelectable |  Qt::ItemIsEnabled;
        }
    }
    return Qt::ItemIsSelectable |  Qt::ItemIsEnabled;
}

Qt::DropActions ToolModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

void ToolModel::objectAdded(QObject *obj)
{
    ProcessorGraphicsBlock* processor = qobject_cast<ProcessorGraphicsBlock*>(obj);
    if(processor)
    {
        ProcessorFactory::instance()->registerProcessor(processor->processor());
        int rows = rowCount();
        QStringList subTitleList = processor->category().split('/');
        ToolItem* parentItem = m_root;
        QModelIndex parentIndex = QModelIndex();
        foreach(QString subTitle, subTitleList)
        {
            bool bFind = false;
            int childrenCount = parentItem->children().count();
            for(int i = 0; i < childrenCount; ++i)
            {
                QModelIndex ind = index(0, 0,parentIndex);
                ToolItem* item = (ToolItem*)(ind.internalPointer());
                if(subTitle == item->text() && item->data() == 0)
                {
                    parentItem = item;
                    parentIndex = ind;
                    bFind = true;
                    break;
                }
            }
            if(bFind == false) //!< 没有找到，就添加ToolItem
            {
                beginInsertRows(parentIndex, childrenCount, childrenCount);
                ToolItem *item = new ToolItem;
                item->setText(subTitle);
                parentItem->addChild(item);
                endInsertRows();
                parentItem = item;
                parentIndex = parentIndex.child(childrenCount, 0);
            }
        }
        // 找到真正的插入位置后就直接插入
        rows = rowCount(parentIndex);// 得到要插入的数据的父亲有多少个孩子

        beginInsertRows(parentIndex, rows, rows + 1);
        ToolItem *pNewItem = new ToolItem(processor);
        pNewItem->setText(processor->name());
        parentItem->addChild(pNewItem);
        endInsertRows();

        QModelIndex posIndex = index(rows, 0, parentIndex);
        emit dataChanged( posIndex, posIndex);
    }
}

ToolItem *ToolModel::itemForIndex(QModelIndex index) const
{
    if(index.isValid())
    {
        if(ToolItem *item = static_cast<ToolItem*>(index.internalPointer()))
        {
            return item;
        }
    }
    return m_root;
}

}
