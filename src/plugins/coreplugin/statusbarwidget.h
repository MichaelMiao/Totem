#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include "core_global.h"
#include "icontext.h"
#include <QPointer>

namespace Core {

class CORE_EXPORT StatusBarWidget : public IContext
{
    Q_OBJECT

public:
    enum StatusBarPosition { First=0, Second=1, Third=2, Last=Third };

    StatusBarWidget(QObject *parent = 0);
    ~StatusBarWidget();

    StatusBarWidget::StatusBarPosition position() const;
    void setPosition(StatusBarWidget::StatusBarPosition position);

private:
    StatusBarWidget::StatusBarPosition m_defaultPosition;
};

} // namespace Core

#endif // STATUSBARWIDGET_H
