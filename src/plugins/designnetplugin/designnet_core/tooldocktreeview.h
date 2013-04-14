#ifndef TOOLDOCKTREEVIEW_H
#define TOOLDOCKTREEVIEW_H

#include <QTreeView>
namespace DesignNet{
/**
 * @brief The ToolDockTreeView class
 *
 * \todo 暂时没有使用树形视图，但树形视图应该比较好。
 */
class ToolDockTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit ToolDockTreeView(QWidget *parent = 0);

signals:

public slots:
protected:
    void startDrag(Qt::DropActions supportedActions);
};
}

#endif // TOOLDOCKTREEVIEW_H
