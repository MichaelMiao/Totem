#ifndef FILTERLINEEDIT_H
#define FILTERLINEEDIT_H

#include "fancylineedit.h"

namespace CustomUI{

class CUSTOMUI_EXPORT FilterLineEdit : public FancyLineEdit
{
    Q_OBJECT
public:
    explicit FilterLineEdit(QWidget *parent = 0);

signals:
    void filterChanged(const QString &);

private slots:
    void slotTextChanged();

private:
    QString m_lastFilterText;

};
}

#endif // FILTERLINEEDIT_H
