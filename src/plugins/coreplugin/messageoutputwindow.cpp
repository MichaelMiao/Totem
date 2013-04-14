#include "messageoutputwindow.h"
#include "coreconstants.h"
#include "icontext.h"
#include "outputwindow.h"
#include <QScrollBar>

using namespace Core::Internal;

MessageOutputWindow::MessageOutputWindow()
{
    m_widget = new Core::OutputWindow(Core::Context(Core::Constants::C_GENERAL_OUTPUT_PANE));
    m_widget->setReadOnly(false);
}

MessageOutputWindow::~MessageOutputWindow()
{
    delete m_widget;
    m_widget = 0;
}

bool MessageOutputWindow::hasFocus() const
{
    return m_widget->hasFocus();
}

bool MessageOutputWindow::canFocus() const
{
    return true;
}

void MessageOutputWindow::setFocus()
{
    m_widget->setFocus();
}

void MessageOutputWindow::clearContents()
{
    m_widget->clear();
}

QWidget *MessageOutputWindow::outputWidget(QWidget *parent)
{
    m_widget->setParent(parent);
    return m_widget;
}

QString MessageOutputWindow::displayName() const
{
    return tr("General Messages");
}

void MessageOutputWindow::visibilityChanged(bool /*b*/)
{
}

void MessageOutputWindow::append(const QString &text)
{
    m_widget->appendText(text);
}

int MessageOutputWindow::priorityInStatusBar() const
{
    return -1;
}

bool MessageOutputWindow::canNext() const
{
    return false;
}

bool MessageOutputWindow::canPrevious() const
{
    return false;
}

void MessageOutputWindow::goToNext()
{

}

void MessageOutputWindow::goToPrev()
{

}

bool MessageOutputWindow::canNavigate() const
{
    return false;
}
