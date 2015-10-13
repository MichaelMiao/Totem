#pragma once
#include <QAbstractItemModel>


class RebarModel : public QAbstractItemModel
{
public:

	int			rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int			columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant	data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant	headerData(int section, Qt::Orientation orientation,
		int role = Qt::DisplayRole) const;
	
	QModelIndex index(int row, int column,
		const QModelIndex &parent = QModelIndex()) const;
	
	QModelIndex parent(const QModelIndex &index) const;

private:


};
