#include "styledbar.h"
#include <QVariant>
#include <QPainter>
#include <QPixmapCache>
#include <QStyle>
#include <QStyleOption>
namespace CustomUI{

StyledBar::StyledBar(QWidget *parent)
    : QWidget(parent)
{
    setProperty("panelwidget", true);
    setProperty("panelwidget_singlerow", true);
    setProperty("lightColored", false);
}

void StyledBar::setSingleRow(bool singleRow)
{
    setProperty("panelwidget_singlerow", singleRow);
}

bool StyledBar::isSingleRow() const
{
    return property("panelwidget_singlerow").toBool();
}

void StyledBar::setLightColored(bool lightColored)
{
    setProperty("lightColored", lightColored);
}

bool StyledBar::isLightColored() const
{
    return property("lightColored").toBool();
}

void StyledBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    QStyleOption option;
    option.rect = rect();
    option.state = QStyle::State_Horizontal;
    style()->drawControl(QStyle::CE_ToolBar, &option, &painter, this);
}
}
