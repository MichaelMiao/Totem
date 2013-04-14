#include "statusbarwidget.h"

#include <QWidget>

using namespace Core;

StatusBarWidget::StatusBarWidget(QObject *parent)
    : IContext(parent), m_defaultPosition(StatusBarWidget::First)
{

}

StatusBarWidget::~StatusBarWidget()
{
    delete m_widget;
}

StatusBarWidget::StatusBarPosition StatusBarWidget::position() const
{
    return m_defaultPosition;
}

void StatusBarWidget::setPosition(StatusBarWidget::StatusBarPosition position)
{
    m_defaultPosition = position;
}
