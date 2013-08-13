#ifndef PATHDIALOGPROPERTYWIDGET_H
#define PATHDIALOGPROPERTYWIDGET_H

#include "ipropertywidget.h"
#include "../property/pathdialogproperty.h"
#include "../designnet_core_global.h"
#include <QListView>
QT_BEGIN_NAMESPACE
class QLineEdit;
class QToolButton;
QT_END_NAMESPACE

namespace DesignNet{

class PathDialogPropertyWidgetPrivate;
class DESIGNNET_CORE_EXPORT PathDialogPropertyWidget : public IPropertyWidget
{
    Q_OBJECT
public:
    explicit PathDialogPropertyWidget(PathDialogProperty* prop, QWidget *parent = 0);
    virtual Core::Id id() const;
public slots:
    void onDataChanged();
protected:
    PathDialogPropertyWidgetPrivate *d;
};
}
#endif // PATHDIALOGPROPERTYWIDGET_H
