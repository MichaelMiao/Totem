#include "pathdialogpropertywidget.h"
#include "designnetconstants.h"
#include "CustomUI/pathlistwidget.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QListWidget>
#include <QToolButton>
#include <QGridLayout>
using namespace CustomUI;
namespace DesignNet{
class PathDialogPropertyWidgetPrivate
{
public:
    PathDialogPropertyWidgetPrivate(PathDialogPropertyWidget *widget);
    PathListWidget *m_pathListWidget;
    PathDialogPropertyWidget *m_widget;
};

PathDialogPropertyWidgetPrivate::PathDialogPropertyWidgetPrivate(PathDialogPropertyWidget *widget)
    : m_widget(widget)
{
}

PathDialogPropertyWidget::PathDialogPropertyWidget(PathDialogProperty *prop, QWidget *parent) :
    IPropertyWidget(prop, parent),
    d(new PathDialogPropertyWidgetPrivate(this))
{
    d->m_pathListWidget = new PathListWidget(prop->m_filters, prop->nameFilters(), prop->m_bSinglePath, parent);
    QList<Utils::Path> listPath = prop->paths();
	QStringList tempPaths;
	for (QList<Utils::Path>::iterator itr = listPath.begin(); itr != listPath.end(); itr++)
		tempPaths << (*itr).m_path;
	d->m_pathListWidget->setPaths(tempPaths);
	addWidget(d->m_pathListWidget);
    connect(d->m_pathListWidget, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
}

Core::Id PathDialogPropertyWidget::id() const
{
    return Constants::PROPERTY_TYPE_PATHDIALOG;
}

void PathDialogPropertyWidget::onDataChanged()
{
    PathDialogProperty * prop = qobject_cast<PathDialogProperty *>(m_property);
    prop->setPaths(d->m_pathListWidget->paths());
}




}
