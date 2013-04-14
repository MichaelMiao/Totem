#include "outputwindow.h"
#include "icore.h"
#include "coreconstants.h"
#include "actionmanager/actionmanager.h"


#include <QAction>
#include <QScrollBar>
using namespace Core;

OutputWindow::OutputWindow(Core::Context context, QWidget *parent)
    : QPlainTextEdit(parent),
      m_formatter(0),
      m_enforceNewline(false),
      m_scrollToBottom(false),
      m_linksActive(true),
      m_mousePressed(false),
      m_maxLineCount(100000)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setFrameShape(QFrame::NoFrame);
    setMouseTracking(true);//默认是不跟踪mouse的（默认只有鼠标按下时跟踪）
    m_outputWindowContext = new Core::IContext;
    m_outputWindowContext->setContext(context);
    m_outputWindowContext->setWidget(this);
    ICore::addContextObject(m_outputWindowContext);//放到对象池当中

    QAction *undoAction     = new QAction(this);
    QAction *redoAction     = new QAction(this);
    QAction *cutAction      = new QAction(this);
    QAction *copyAction     = new QAction(this);
    QAction *pasteAction    = new QAction(this);
    QAction *selectAllAction = new QAction(this);

    Core::ActionManager *am = ICore::actionManager();
    am->registerAction(undoAction, Core::Constants::UNDO, context);
    am->registerAction(redoAction, Core::Constants::REDO, context);
    am->registerAction(cutAction, Core::Constants::CUT, context);
    am->registerAction(copyAction, Core::Constants::COPY, context);
    am->registerAction(pasteAction, Core::Constants::PASTE, context);
    am->registerAction(selectAllAction, Core::Constants::SELECTALL, context);

    connect(undoAction, SIGNAL(triggered()), this, SLOT(undo()));
    connect(redoAction, SIGNAL(triggered()), this, SLOT(redo()));
    connect(cutAction, SIGNAL(triggered()), this, SLOT(cut()));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(copy()));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(paste()));
    connect(selectAllAction, SIGNAL(triggered()), this, SLOT(selectAll()));

    connect(this, SIGNAL(undoAvailable(bool)),
            undoAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(redoAvailable(bool)),
            redoAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(copyAvailable(bool)),
            cutAction, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(copyAvailable(bool)),
            copyAction, SLOT(setEnabled(bool)));

    undoAction->setEnabled(false);
    redoAction->setEnabled(false);
    cutAction->setEnabled(false);
    copyAction->setEnabled(false);
}

OutputWindow::~OutputWindow()
{
    Core::ICore::removeContextObject(m_outputWindowContext);
    delete m_outputWindowContext;
}

Utils::OutputFormatter *OutputWindow::formatter() const
{
    return m_formatter;
}

void OutputWindow::setFormatter(Utils::OutputFormatter *formatter)
{
    m_formatter = formatter;
    m_formatter->setPlainTextEdit(this);
}

void OutputWindow::appendMessage(const QString &output, Utils::OutputFormat format)
{
    QString out = output;
    out.remove(QLatin1Char('\r'));
    setMaximumBlockCount(m_maxLineCount);
    const bool atBottom = isScrollbarAtBottom();

    if (format == Utils::ErrorMessageFormat || format == Utils::NormalMessageFormat)
    {
        m_formatter->appendMessage(doNewlineEnfocement(out), format);
    }
    else
    {
        bool sameLine = format == Utils::StdOutFormat
                     || format == Utils::StdErrFormat;

        if (sameLine)
        {
            m_scrollToBottom = true;

            int newline = -1;
            bool enforceNewline = m_enforceNewline;
            m_enforceNewline = false;

            if (!enforceNewline) //需要测试一下
            {
                newline = out.indexOf(QLatin1Char('\n'));
                moveCursor(QTextCursor::End);
                if (newline != -1)
                    m_formatter->appendMessage(out.left(newline), format);
            }

            QString s = out.mid(newline+1);
            if (s.isEmpty())
            {
                m_enforceNewline = true;
            }
            else
            {
                if (s.endsWith(QLatin1Char('\n')))
                {
                    m_enforceNewline = true;
                    s.chop(1);
                }
                m_formatter->appendMessage(QLatin1Char('\n') + s, format);
            }
        }
        else
        {
            m_formatter->appendMessage(doNewlineEnfocement(out), format);
        }
    }

    if (atBottom)
        scrollToBottom();
    enableUndoRedo();
}

void OutputWindow::appendText(const QString &textIn, const QTextCharFormat &format)
{
    QString text = textIn;
    text.remove(QLatin1Char('\r'));
    if (m_maxLineCount > 0 && document()->blockCount() > m_maxLineCount)
        return;
    const bool atBottom = isScrollbarAtBottom();
    QTextCursor cursor = QTextCursor(document());
    cursor.movePosition(QTextCursor::End);
    cursor.beginEditBlock();
    cursor.insertText(doNewlineEnfocement(text), format);

    if (m_maxLineCount > 0 && document()->blockCount() > m_maxLineCount)
    {
        QTextCharFormat tmp;
        tmp.setFontWeight(QFont::Bold);
        cursor.insertText(tr("Additional output omitted\n"), tmp);
    }

    cursor.endEditBlock();
    if (atBottom)
        scrollToBottom();
}

void OutputWindow::grayOutOldContent()
{
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    QTextCharFormat endFormat = cursor.charFormat();

    cursor.select(QTextCursor::Document);

    QTextCharFormat format;
    const QColor bkgColor = palette().base().color();
    const QColor fgdColor = palette().text().color();
    double bkgFactor = 0.50;
    double fgdFactor = 1.-bkgFactor;
    format.setForeground(QColor((bkgFactor * bkgColor.red() + fgdFactor * fgdColor.red()),
                             (bkgFactor * bkgColor.green() + fgdFactor * fgdColor.green()),
                             (bkgFactor * bkgColor.blue() + fgdFactor * fgdColor.blue()) ));
    cursor.mergeCharFormat(format);

    cursor.movePosition(QTextCursor::End);
    cursor.setCharFormat(endFormat);
    cursor.insertBlock(QTextBlockFormat());
}

void OutputWindow::clear()
{
    m_enforceNewline = false;
    QPlainTextEdit::clear();
}

void OutputWindow::showEvent(QShowEvent *e)
{
    QPlainTextEdit::showEvent(e);
    if (m_scrollToBottom) {
        verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    }
    m_scrollToBottom = false;
}

void OutputWindow::scrollToBottom()
{
    //多行文本处理
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
    // QPlainTextEdit destroys the first calls value in case of multiline
    // text, so make sure that the scroll bar actually gets the value set.
    // Is a noop if the first call succeeded.
    verticalScrollBar()->setValue(verticalScrollBar()->maximum());
}

void OutputWindow::setMaxLineCount(int count)
{
    m_maxLineCount = count;
    setMaximumBlockCount(m_maxLineCount);//会使undo redo失效
}

void OutputWindow::setWordWrapEnabled(bool wrap)
{
    if (wrap)
        setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    else
        setWordWrapMode(QTextOption::NoWrap);
}

bool OutputWindow::isScrollbarAtBottom() const
{
    return verticalScrollBar()->value() == verticalScrollBar()->maximum();
}

void OutputWindow::mousePressEvent(QMouseEvent *e)
{
    m_mousePressed = true;
    QPlainTextEdit::mousePressEvent(e);
}

void OutputWindow::mouseReleaseEvent(QMouseEvent *e)
{
    m_mousePressed = false;

    if (m_linksActive)
    {
        const QString href = anchorAt(e->pos());
        if (m_formatter)
            m_formatter->handleLink(href);
    }
    m_linksActive = true;

    QPlainTextEdit::mouseReleaseEvent(e);
}

void OutputWindow::mouseMoveEvent(QMouseEvent *e)
{
    // 使超链接无效，因为用户是选择了含有超链接的文本
    if (m_mousePressed && textCursor().hasSelection())
        m_linksActive = false;

    if (!m_linksActive || anchorAt(e->pos()).isEmpty())
        viewport()->setCursor(Qt::IBeamCursor);
    else
        viewport()->setCursor(Qt::PointingHandCursor);
    QPlainTextEdit::mouseMoveEvent(e);
}

void OutputWindow::resizeEvent(QResizeEvent *e)
{
    bool atBottom = isScrollbarAtBottom();
    QPlainTextEdit::resizeEvent(e);
    if (atBottom)
        scrollToBottom();
}

void OutputWindow::keyPressEvent(QKeyEvent *ev)
{
    QPlainTextEdit::keyPressEvent(ev);

    //使用Ctrl+Home 或者 Ctrl+End滚动
    if (ev->matches(QKeySequence::MoveToStartOfDocument))
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMinimum);
    else if (ev->matches(QKeySequence::MoveToEndOfDocument))
        verticalScrollBar()->triggerAction(QAbstractSlider::SliderToMaximum);
}

void OutputWindow::enableUndoRedo()
{
    setMaximumBlockCount(0);
    setUndoRedoEnabled(true);
}

QString OutputWindow::doNewlineEnfocement(const QString &out)
{
    m_scrollToBottom = true;
    QString s = out;
    if (m_enforceNewline)
    {
        s.prepend(QLatin1Char('\n'));
        m_enforceNewline = false;
    }

    if (s.endsWith(QLatin1Char('\n')))
    {
        m_enforceNewline = true;
        s.chop(1);
    }

    return s;
}
