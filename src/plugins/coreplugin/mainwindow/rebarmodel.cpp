#include "stdafx.h"
#include "rebarmodel.h"


int RebarModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 0;
}

int RebarModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 0;
}

QVariant RebarModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
{
	return 0;
}

Qt::ItemFlags RebarModel::flags(const QModelIndex &index) const
{
	return 0;
}

QModelIndex RebarModel::index(int row, int column, const QModelIndex &parent) const
{
	return QModelIndex();
}

QModelIndex RebarModel::parent(const QModelIndex &index) const
{
	return QModelIndex();
}
