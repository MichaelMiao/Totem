#include "outputformatter.h"

#include <QPalette>
#include <QColor>
#include <QPlainTextEdit>
#include <QTextCharFormat>

using namespace Utils;
OutputFormatter::OutputFormatter(QObject *parent) :
    QObject(parent),
    m_plainTextEdit(0)
{
    m_formats = new QTextCharFormat[NumberOfFormats];
}

OutputFormatter::~OutputFormatter()
{
    if (m_formats)
    {
        delete m_formats;
        m_formats = 0;
    }
}

QPlainTextEdit *OutputFormatter::plainTextEdit() const
{
    return m_plainTextEdit;
}

void OutputFormatter::setPlainTextEdit(QPlainTextEdit *plainText)
{
    m_plainTextEdit = plainText;
    initFormats();
}

QFont OutputFormatter::font() const
{
    return m_font;
}

void OutputFormatter::setFont(const QFont &font)
{
    m_font = font;
    initFormats();
}

void OutputFormatter::appendMessage(const QString &text, OutputFormat format)
{
    QTextCursor cursor(m_plainTextEdit->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(text, m_formats[format]);
}

void OutputFormatter::handleLink(const QString &href)
{
    Q_UNUSED(href);
}

void OutputFormatter::initFormats()
{
    if (!m_plainTextEdit)
    {
        return;
    }
    QPalette p = m_plainTextEdit->palette();

    QFont boldFont = m_font;
    boldFont.setBold(true);



    // NormalMessageFormat
    m_formats[NormalMessageFormat].setFont(boldFont);
    m_formats[NormalMessageFormat].setForeground(
                mixColors(p.color(QPalette::Text), QColor(Qt::blue)));

    // ErrorMessageFormat
    m_formats[ErrorMessageFormat].setFont(boldFont);
    m_formats[ErrorMessageFormat].setForeground(
                mixColors(p.color(QPalette::Text), QColor(Qt::red)));

    // StdOutFormat
    m_formats[StdOutFormat].setFont(m_font);
    m_formats[StdOutFormat].setForeground(p.color(QPalette::Text));

    // StdErrFormat
    m_formats[StdErrFormat].setFont(m_font);
    m_formats[StdErrFormat].setForeground(mixColors(p.color(QPalette::Text), QColor(Qt::red)));

}

void OutputFormatter::clearLastLine()
{
    QTextCursor cursor(m_plainTextEdit->document());
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    cursor.removeSelectedText();
}

QTextCharFormat OutputFormatter::charFormat(OutputFormat format) const
{
    return m_formats[format];
}
//三分之一 a，三分之二 b
QColor OutputFormatter::mixColors(const QColor &a, const QColor &b)
{
    return QColor((a.red() + 2 * b.red()) / 3, (a.green() + 2 * b.green()) / 3,
                  (a.blue() + 2* b.blue()) / 3, (a.alpha() + 2 * b.alpha()) / 3);

}