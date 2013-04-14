#include "tooldockwidget.h"
#include "CustomUI/filterlineedit.h"


#include "toolmodel.h"
#include "tooldocktreeview.h"

#include <QMessageBox>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QAbstractItemModel>
#include <QScrollArea>

using namespace CustomUI;
namespace DesignNet{


class ToolDockWidgetPrivate
{
public:
    ToolDockWidgetPrivate(ToolDockWidget* pWidget);

    ToolDockTreeView*               m_processorTreeWidget;
    CustomUI::FilterLineEdit*       m_filterLineEdit;
    QVBoxLayout *                   m_layout;

    ToolModel*                      m_Model;    //!< 所关联的Model
    ToolDockWidget*                 q;
};

ToolDockWidgetPrivate::ToolDockWidgetPrivate(ToolDockWidget *pWidget)
    : q(pWidget)
{
    m_Model                 = ToolModel::instance();
    m_processorTreeWidget   = new ToolDockTreeView(pWidget);
    m_processorTreeWidget->setModel(m_Model);

    m_filterLineEdit    = new FilterLineEdit(q);

    m_layout            = new QVBoxLayout;
    m_layout->addWidget(m_filterLineEdit);
    m_layout->addWidget(m_processorTreeWidget);

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    q->setLayout(m_layout);

}

ToolDockWidget::ToolDockWidget(QWidget *parent) :
    QWidget(parent),
    d(new ToolDockWidgetPrivate(this))
{
    setMinimumSize(100, 100);
}

void ToolDockWidget::onMinMax(bool &bMin)
{
    QMessageBox box;
    box.setText(tr("%1").arg(bMin));
    box.exec();
}

}
