#ifndef UICOLLAPSIBLEWIDGET_H
#define UICOLLAPSIBLEWIDGET_H

#include <QWidget>

#include "customui_global.h"

#include <QHash>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace CustomUI{

class CollapsibleButton;
class uiCollapsibleWidgetPrivate;
class CUSTOMUI_EXPORT uiCollapsibleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit uiCollapsibleWidget(Qt::Alignment alignment = Qt::AlignTop, QWidget *parent = 0);
    void addRegion(const QString &text, QWidget *widget = 0);
    void addRegion(const QString &text, QLayout *pLayout);
    void insertRegion(const QString &text, QWidget *widget = 0, int index = 0);
    void insertRegion(const QString &text, QLayout *pLayout, int index = 0);

    void removeRegion(const int &index);

	QSize sizeHint() const override;

signals:

public slots:
protected:
    uiCollapsibleWidgetPrivate*                d;
};
}

#endif // UICOLLAPSIBLEWIDGET_H
