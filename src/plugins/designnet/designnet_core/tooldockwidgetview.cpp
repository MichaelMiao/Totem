#include "tooldockwidgetview.h"
#include "tooldockwidget.h"
#include "CustomUI/flowlayout.h"
#include "toolitem.h"
#include "designnetconstants.h"
#include "designnetbase/processor.h"

#include <QEvent>
#include <QResizeEvent>
#include <QButtonGroup>
#include <QImage>
#include <QScrollArea>
#include <QScrollBar>
#include <QToolButton>
#include <QHash>
#include <QDrag>
#include <QMimeData>
const int ExtraWidth = 3;
using namespace CustomUI;
namespace DesignNet{

ToolDockWidgetView::ToolDockWidgetView(QWidget *parent) :
    uiCollapsibleWidget(Qt::AlignTop, parent)
{
    m_toolWidget = qobject_cast<ToolDockWidget*>(parent);
    m_buttonGroup       = new QButtonGroup(parent);

    setFocusPolicy(Qt::WheelFocus);
    setMinimumSize(minimumSizeHint());
    connect(m_buttonGroup, SIGNAL(buttonPressed(QAbstractButton*)),
            this, SLOT(clicked(QAbstractButton*)));
}

void ToolDockWidgetView::onToolLoaded(QModelIndex index)
{
//    ToolItem *pItem = static_cast<ToolItem*>(index.internalPointer());
//    DesignNetTool*pTool = pItem->data();
//    QImage  icon        = pTool->getIcon();
//    QString title       = pTool->getTitle();
//    QString name        = pTool->getName();
//    QString toolTip     = pTool->getTooltip();
//    FlowLayout *pLayout = m_subLayout.value(title);
//    if(!pLayout)
//    {
//        pLayout = new FlowLayout;
//        m_subLayout.insert(title, pLayout);
//        addRegion(title, pLayout);
//    }
//    QToolButton *pButton = new QToolButton(this);

//    pButton->setIcon(QIcon(QPixmap::fromImage(icon)));
//    pButton->setAutoRaise(true);
//    pButton->setToolTip(toolTip);
//    m_buttonGroup->addButton(pButton);
//    pLayout->addWidget(pButton);
//    m_toolIndexs.insert(pButton, index);
}

bool ToolDockWidgetView::eventFilter(QObject *obj, QEvent *event)
{
//    if(obj == m_toolWidget->scrollArea() && event->type() == QEvent::Resize)
//    {
//        if(QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event))
//        {
//            QSize size = resizeEvent->size();
//            int width = size.width() - ExtraWidth
//                    - m_toolWidget->scrollArea()->verticalScrollBar()->width();
//            size.setWidth(width);
//            size.setHeight(sizeHint().height());
//            resize(size);
//        }
//    }
    return QWidget::eventFilter(obj, event);
}

void ToolDockWidgetView::clicked(QAbstractButton *button)
{
    QToolButton *pButton = qobject_cast<QToolButton*>(button);
    QModelIndex index = m_toolIndexs.value(pButton);
    QDrag *drag = new QDrag(this);
    QMimeData *data = new QMimeData();
//    DesignNetTool*pTool = (DesignNetTool*)(index.data().value<void*>());
//    ProcessorGraphicsBlock *processor = pTool->getGraphicsProcessor();
//    data->setData(Constants::MIME_TYPE_TOOLITEM, processor->metaObject()->className());
    drag->setMimeData(data);

    drag->start(Qt::CopyAction);
}

}
