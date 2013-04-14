#ifndef TOOLDOCKWIDGETVIEW_H
#define TOOLDOCKWIDGETVIEW_H

#include "CustomUI/uicollapsiblewidget.h"
#include <QModelIndex>
#include <QAbstractItemView>

QT_BEGIN_NAMESPACE
class QButtonGroup;
class QAbstractButton;
class QToolButton;
QT_END_NAMESPACE

namespace CustomUI{
class FlowLayout;
}
namespace DesignNet{
class ToolDockWidget;
class DesignNetTool;
class ToolDockWidgetView : public CustomUI::uiCollapsibleWidget
{
    Q_OBJECT
public:
    explicit ToolDockWidgetView(QWidget *parent = 0);


signals:
    void clicked(QModelIndex &index);
public slots:
    void clicked(QAbstractButton* button);
    void onToolLoaded(QModelIndex index);
    bool eventFilter(QObject *obj, QEvent *event);
protected:
    QHash<QString, CustomUI::FlowLayout*>       m_subLayout;    //!< 所有的子布局
    QButtonGroup*                               m_buttonGroup;  //!< 所有button为一个QButtonGroup
    QHash<QToolButton*, QModelIndex>            m_toolIndexs;   //!< 所有button对应的QModelIndex
    bool            m_bDirty;       //!< 是否需要更新视图
    int             m_idealWidth;   //!< 推荐宽度
    int             m_idealHeight;  //!< 推荐高度
    ToolDockWidget* m_toolWidget;
};
}

#endif // TOOLDOCKWIDGETVIEW_H
