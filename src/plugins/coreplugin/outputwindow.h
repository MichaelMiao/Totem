#ifndef OUTPUTWINDOW_H
#define OUTPUTWINDOW_H

#include "core_global.h"

#include "utils/outputformatter.h"
#include "icontext.h"

#include <QPlainTextEdit>

namespace Core{
class IContext;
class CORE_EXPORT OutputWindow : public QPlainTextEdit
{
    Q_OBJECT

public:
    OutputWindow(Core::Context context, QWidget *parent = 0);
    virtual ~OutputWindow();

    Utils::OutputFormatter* formatter() const;
    void setFormatter(Utils::OutputFormatter *formatter);

    void appendMessage(const QString &output, Utils::OutputFormat format);
    /// appends a \p text using \p format without using formater
    void appendText(const QString &textIn, const QTextCharFormat &format = QTextCharFormat());

    void grayOutOldContent();
    void clear();

    void showEvent(QShowEvent *e);

    void scrollToBottom();

    void setMaxLineCount(int count);
    int maxLineCount() const { return m_maxLineCount; }

public slots:
    void setWordWrapEnabled(bool wrap);

protected:
    bool isScrollbarAtBottom() const;

    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void keyPressEvent(QKeyEvent *ev);

private:
    void enableUndoRedo();
    QString doNewlineEnfocement(const QString &out);

    Core::IContext *m_outputWindowContext;
    Utils::OutputFormatter *m_formatter;

    bool m_enforceNewline;
    bool m_scrollToBottom;
    bool m_linksActive;
    bool m_mousePressed;
    int m_maxLineCount;
};

}

#endif // OUTPUTWINDOW_H
