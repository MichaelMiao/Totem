#ifndef TREEWIDGETCOLUMNSTRETCHER_H
#define TREEWIDGETCOLUMNSTRETCHER_H

#include "utils_global.h"
#include <QObject>

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE

namespace Utils {

class TOTEM_UTILS_EXPORT TreeWidgetColumnStretcher : public QObject
{
    const int m_columnToStretch;
public:
    explicit TreeWidgetColumnStretcher(QTreeWidget *treeWidget,
                                       int columnToStretch);

    virtual bool eventFilter(QObject *obj, QEvent *ev);
};

}

#endif // TREEWIDGETCOLUMNSTRETCHER_H