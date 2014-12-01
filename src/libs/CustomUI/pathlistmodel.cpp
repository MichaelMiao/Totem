#include "pathlistmodel.h"
#include <QFileInfo>
#include <QDir>
#include <QFileIconProvider>

namespace CustomUI{

PathListModel::PathListModel( bool bSingleSelection /*= true*/, QObject *parent /*= 0*/ )
	: QAbstractTableModel(parent), m_bSingleSelection(bSingleSelection)
{

}

int PathListModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_paths.size();
}

int PathListModel::columnCount(const QModelIndex &) const
{
    return 2;
}

Qt::ItemFlags PathListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid() && index.column() == 1)
        theFlags |= Qt::ItemIsUserCheckable;
    return theFlags;
}

void PathListModel::addPath(const QString &strPath, bool bRecursion)
{
    Path path;
    path.m_path     = strPath;
    path.bRecursion = bRecursion;
	if(!m_bSingleSelection || m_paths.size() == 0)
	{
		beginInsertRows(QModelIndex(), 0, 0);
		m_paths.prepend(path);
		endInsertRows();
	}
	else
	{
		m_paths[0] = path;
	}

    QModelIndex posIndex = index(0, 0, QModelIndex());
    emit dataChanged( posIndex, posIndex);
}

QModelIndex PathListModel::index(int row, int column, const QModelIndex &) const
{
    if(row >= m_paths.count() || column > 1)
        return QModelIndex();
    return createIndex(row, column);
}

QVariant PathListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() ||
            index.column() < 0 ||
            index.row() < 0 ||
            index.row() >= m_paths.count() ||
            index.column() > 1)
        return QVariant();
    const Path &path = m_paths.at(index.row());
    if(index.column() == 0)
    {
        if(role == Qt::DisplayRole)
            return path.m_path;
        else if(role == Qt::DecorationRole)
        {
            QFileInfo info(path.m_path);
            QFileIconProvider provider;
            return provider.icon(info);
        }
        else if(role == Qt::ToolTipRole)
        {
            return path.m_path;
        }
    }
    else if(index.column() == 1)
    {
        bool bret = path.bRecursion;
        if(role == Qt::ToolTipRole)
        {
            if(bret)
                return tr("The path is recursive but you can cancel it by making it checked");
            else
                return tr("The path is not recursive but you can enable it by making it unchecked");
        }
        else if(role == Qt::CheckStateRole)
        {
            return bret ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

QVariant PathListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();
    if(orientation == Qt::Horizontal)
    {
        switch(section){
        case 0: return tr("Path");
        case 1: return tr("Recursive");
        default: return QVariant();
        }
    }
    return QVariant();
}

bool PathListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() ||index.column() != 1 || role != Qt::CheckStateRole)
        return false;
    bool bRecursive = value.toBool();
    if(bRecursive != m_paths.at(index.row()).bRecursion)
    {
        Path &p = m_paths[index.row()];
        p.bRecursion = bRecursive;
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool PathListModel::insertRows(int row, int count, const QModelIndex &)
{
    qDebug() << "insertRows";
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_paths.insert(row, Path());
    endInsertRows();
    return true;
}

bool PathListModel::removeRows(int row, int count, const QModelIndex &)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = 0; i < count; ++i)
        m_paths.removeAt(row);
    endRemoveRows();
    return true;
}

}
