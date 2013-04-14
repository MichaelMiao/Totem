#ifndef PATHLISTMODEL_H
#define PATHLISTMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "customui_global.h"
#include "utils/fileutils.h"
using namespace Utils;

namespace CustomUI{

class CUSTOMUI_EXPORT PathListModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class PathListWidget;
public:
    explicit PathListModel(bool bSingleSelection = true, QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &/*parent*/) const;

    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    void addPath(const QString& strPath, bool bRecursion = false);
    QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool setHeaderData(int, Qt::Orientation, const QVariant&, int=Qt::EditRole) { return false; }

    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    bool insertRows( int row, int count, const QModelIndex & parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

signals:
    
public slots:
protected:
    QList<Path> m_paths;
	bool m_bSingleSelection;
};
}

#endif // PATHLISTMODEL_H
