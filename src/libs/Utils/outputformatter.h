#ifndef OUTPUTFORMATTER_H
#define OUTPUTFORMATTER_H

#include "utils_global.h"
#include <QObject>
#include <QFont>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QTextCharFormat;
class QColor;
QT_END_NAMESPACE

namespace Utils{

enum OutputFormat{
    NormalMessageFormat,
    ErrorMessageFormat,
    StdOutFormat,
    StdErrFormat,
    NumberOfFormats
};

class TOTEM_UTILS_EXPORT OutputFormatter : public QObject
{
    Q_OBJECT
public:
    explicit OutputFormatter(QObject *parent = 0);
    virtual ~OutputFormatter();
    QPlainTextEdit *plainTextEdit() const;
    void setPlainTextEdit(QPlainTextEdit *plainText);//!< 将会进行初始化处理

    QFont font() const;
    void setFont(const QFont &font);

    virtual void appendMessage(const QString &text, OutputFormat format);
    virtual void handleLink(const QString &href);

protected:
    void initFormats();
    void clearLastLine();
    QTextCharFormat charFormat(OutputFormat format) const;

    static QColor mixColors(const QColor &a, const QColor &b);

private:
    QPlainTextEdit *m_plainTextEdit;
    QTextCharFormat *m_formats;
    QFont m_font;
};
}


#endif // OUTPUTFORMATTER_H
