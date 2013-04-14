#ifndef FANCYLINEEDIT_H
#define FANCYLINEEDIT_H

#include "customui_global.h"

#include <QLineEdit>
#include <QAbstractButton>
namespace CustomUI{
class FancyLineEditPrivate;

class CUSTOMUI_EXPORT FancyLineEdit : public QLineEdit
{
    Q_OBJECT
    Q_ENUMS(Side)
public:
    enum Side {Left = 0, Right = 1};

    explicit FancyLineEdit(QWidget *parent = 0);
    ~FancyLineEdit();

    QPixmap buttonPixmap(Side side) const;
    void setButtonPixmap(Side side, const QPixmap &pixmap);

    QMenu *buttonMenu(Side side) const;
    void setButtonMenu(Side side, QMenu *buttonMenu);

    void setButtonVisible(Side side, bool visible);
    bool isButtonVisible(Side side) const;

    void setButtonToolTip(Side side, const QString &);
    void setButtonFocusPolicy(Side side, Qt::FocusPolicy policy);

    // Set whether tabbing in will trigger the menu.
    void setMenuTabFocusTrigger(Side side, bool v);
    bool hasMenuTabFocusTrigger(Side side) const;

    // 如果Text空，设置是否隐藏Button
    void setAutoHideButton(Side side, bool h);
    bool hasAutoHideButton(Side side) const;

signals:
    void buttonClicked(CustomUI::FancyLineEdit::Side side);
    void leftButtonClicked();
    void rightButtonClicked();
private slots:
    void checkButtons(const QString & text);
    void iconClicked();

protected:
    virtual void resizeEvent(QResizeEvent *e);

private:
    void updateMargins();
    void updateButtonPositions();
    friend class CustomUI::FancyLineEditPrivate;

    FancyLineEditPrivate *d;
    QString m_oldText;

};
}

#endif // FANCYLINEEDIT_H
