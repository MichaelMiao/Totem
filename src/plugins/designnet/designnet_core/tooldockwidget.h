#ifndef TOOLDOCKWIDGET_H
#define TOOLDOCKWIDGET_H

#include <QWidget>
#include <QModelIndex>
QT_BEGIN_NAMESPACE
class QButtonGroup;
class QAbstractItemModel;
class QScrollArea;
QT_END_NAMESPACE

namespace CustomUI{
class uiCollapsibleWidget;
class FilterLineEdit;
}
namespace DesignNet{
class DesignNetTool;
class ToolDockWidgetPrivate;
//工具Dock窗口
class ToolModel;
class ToolDockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolDockWidget(QWidget *parent = 0);

signals:
    void clicked(QModelIndex&);
public slots:
    void onMinMax(bool &bMin);

private:
    ToolDockWidgetPrivate*      d;
};
}

#endif // TOOLDOCKWIDGET_H
