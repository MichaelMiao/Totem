#ifndef PATHLISTWIDGET_H
#define PATHLISTWIDGET_H

#include <QWidget>
#include <QDir>
#include <QModelIndex>
#include "customui_global.h"
#include "pathlistmodel.h"
QT_BEGIN_NAMESPACE
class QGridLayout;
class QTableView;
class QToolButton;
QT_END_NAMESPACE

namespace CustomUI{
class FancyLineEdit;

class CUSTOMUI_EXPORT PathListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PathListWidget(QDir::Filters filters = QDir::AllEntries,
                            const QStringList &nameFilters = QStringList(),
                            bool bSingleSelection = false,
                            QWidget *parent = 0);
    void setFilters(QDir::Filters filters);
    void setNameFilters(const QStringList &filterList);
    QList<Path> paths() const;
signals:
    void dataChanged();
public slots:
    void addPath();
    void deletePath();
    void selectChanged(QModelIndex index);
protected:
    PathListModel*  m_model;
    QGridLayout *   m_gridLayout;           //!< 网格布局
    FancyLineEdit*  m_pathLineEdit;         //!< 路径编辑框，并提供历史记录
    QTableView *    m_tableView;            //!< 表view
    QToolButton*    m_buttonAddFolder;      //!< 添加文件按钮
    QToolButton*    m_buttonAddFile;        //!< 添加文件夹按钮
    QDir::Filters   m_filters;              //!< 该列表添加的路径类型
    QStringList     m_nameFilters;          //!< 文件类型过滤
    bool            m_bSingleSelection;     //!< 是否单选
};
}

#endif // PATHLISTWIDGET_H
