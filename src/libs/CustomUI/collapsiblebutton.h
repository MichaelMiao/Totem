#ifndef COLLAPSIBLEBUTTON_H
#define COLLAPSIBLEBUTTON_H

#include <QAbstractButton>
#include "customui_global.h"

QT_BEGIN_NAMESPACE
class QStyleOptionButton;
QT_END_NAMESPACE

namespace CustomUI{
class CollapsibleButtonPrivate;
class CUSTOMUI_EXPORT CollapsibleButton : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(bool collapsed READ isCollapsed WRITE setCollapsed NOTIFY contentsCollapsed)
    Q_PROPERTY(Qt::Alignment buttonTextAlignment READ buttonTextAlignment WRITE setButtonTextAlignment)
    Q_PROPERTY(Qt::Alignment indicatorAlignment READ indicatorAlignment WRITE setIndicatorAlignment)

public:
    explicit CollapsibleButton(QWidget* parent = 0);
    CollapsibleButton(const QString& text, QWidget *parent = 0);
    virtual ~CollapsibleButton();
    ///
    /// collapsed set/get
    bool isCollapsed();
    void setCollapsed(bool bCollapsed = true);

    ///
    /// buttonTextAlginment
    Qt::Alignment buttonTextAlignment();
    void setButtonTextAlignment(Qt::Alignment alignment);

    ///
    /// indicatorAlignment
    Qt::Alignment indicatorAlignment()const;
    void setIndicatorAlignment(Qt::Alignment indicatorAlignment);

    virtual QSize minimumSizeHint()const;
    virtual QSize sizeHint()const;

    virtual bool event(QEvent* event);

    virtual bool eventFilter(QObject* child, QEvent* e);
    virtual void setVisible(bool bShow);

signals:
    void contentsCollapsed(bool);

public slots:
    virtual void collapse(bool c);
    virtual void onToggled(bool clicked = false);
protected:
    virtual void paintEvent(QPaintEvent*_event);
    virtual void childEvent(QChildEvent* c);

    virtual bool hitButton(const QPoint & pos) const;
    virtual QSize buttonSizeHint() const;

    virtual void initStyleOption(QStyleOptionButton* option)const;
    friend class CollapsibleButtonPrivate;
    CollapsibleButtonPrivate*   d;
};
}

#endif // COLLAPSIBLEBUTTON_H
