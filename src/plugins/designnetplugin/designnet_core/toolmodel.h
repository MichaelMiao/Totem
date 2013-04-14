#ifndef TOOLMODEL_H
#define TOOLMODEL_H

#include <QAbstractItemModel>
namespace DesignNet{

class ToolItem;
class Processor;
class ToolModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    ToolModel(QObject *parent);
    ~ToolModel();
    static ToolModel* instance();
    virtual int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant	data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

    virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const ;
    virtual QModelIndex	parent ( const QModelIndex & index ) const ;
    virtual int	rowCount ( const QModelIndex & parent = QModelIndex() ) const ;
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    Qt::DropActions supportedDropActions() const;
public slots:
    void objectAdded(QObject* obj);
protected:
    ToolItem*   itemForIndex(QModelIndex index) const;
    ToolItem*   m_root;
    static ToolModel*  m_instance;
};
}
#endif // TOOLMODEL_H
