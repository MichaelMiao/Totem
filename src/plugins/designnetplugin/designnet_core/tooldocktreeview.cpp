#include "tooldocktreeview.h"
#include "designnetconstants.h"
#include "ProcessorGraphicsBlock.h"
#include <QModelIndex>
#include <QMimeData>
#include <QDrag>
#include <QHeaderView>
namespace DesignNet{

ToolDockTreeView::ToolDockTreeView(QWidget *parent) :
    QTreeView(parent)
{
    setFocusPolicy(Qt::WheelFocus);
    setMinimumSize(minimumSizeHint());
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragOnly);
	resizeColumnToContents(0);
}

void ToolDockTreeView::startDrag(Qt::DropActions /*supportedActions*/)
{
    qDebug() << "QAbstractItemView::startDrag; begin";
    QModelIndex index = selectionModel()->currentIndex();
    ProcessorGraphicsBlock *pBlock = (ProcessorGraphicsBlock *)index.data(Qt::UserRole).value<void*>();
    if(pBlock)
    {
        QMimeData *mimeData = new QMimeData;

        mimeData->setData(Constants::MIME_TYPE_TOOLITEM,
                          pBlock->typeID().toString().toStdString().c_str());

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec(Qt::MoveAction);
        delete drag;
    }
}
}
