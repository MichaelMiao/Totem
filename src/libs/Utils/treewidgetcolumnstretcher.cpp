﻿#include "treewidgetcolumnstretcher.h"
#include <QTreeWidget>
#include <QHideEvent>
#include <QHeaderView>

namespace Utils{

TreeWidgetColumnStretcher::TreeWidgetColumnStretcher(QTreeWidget *treeWidget, int columnToStretch)
        : QObject(treeWidget->header()),
          m_columnToStretch(columnToStretch)
{
    parent()->installEventFilter(this);
    QHideEvent fake;
    TreeWidgetColumnStretcher::eventFilter(parent(), &fake);
}

bool TreeWidgetColumnStretcher::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == parent())
    {
        if (ev->type() == QEvent::Show)
        {
            QHeaderView *hv = qobject_cast<QHeaderView*>(obj);
            for (int i = 0; i < hv->count(); ++i)
				hv->setSectionResizeMode(i, QHeaderView::Interactive);
        }
        else if (ev->type() == QEvent::Hide)
        {
            QHeaderView *hv = qobject_cast<QHeaderView*>(obj);
            for (int i = 0; i < hv->count(); ++i)
				hv->setSectionResizeMode(i, i == m_columnToStretch ? QHeaderView::Stretch : QHeaderView::ResizeToContents);
        }
        else if (ev->type() == QEvent::Resize)
        {
            QHeaderView *hv = qobject_cast<QHeaderView*>(obj);
            if (hv->sectionResizeMode(m_columnToStretch) == QHeaderView::Interactive)
            {
                QResizeEvent *re = static_cast<QResizeEvent*>(ev);
                int diff = re->size().width() - re->oldSize().width() ;
                hv->resizeSection(m_columnToStretch, qMax(32, hv->sectionSize(1) + diff));
            }
        }
    }
    return false;
}

}