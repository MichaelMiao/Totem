#ifndef FILEUTILS_H
#define FILEUTILS_H

#include "utils_global.h"
#include "XML/xmlserializable.h"
#include "XML/xmldeserializer.h"
#include "XML/xmlserializer.h"

#include <QCoreApplication>
#include <QXmlStreamWriter>
#include <QIODevice>

QT_BEGIN_NAMESPACE
class QFile;
class QFileInfo;
class QTemporaryFile;
class QWidget;
class QTextStream;
class QDataStream;
class QDateTime;
class QDir;
QT_END_NAMESPACE
namespace Utils{

class TOTEM_UTILS_EXPORT FileName : private QString
{
public:
    FileName();
    explicit FileName(const QFileInfo &info);
    QFileInfo toFileInfo() const;
    static FileName fromString(const QString &filename);
    static FileName fromUserInput(const QString &filename);
    QString toString() const;
    QString toUserOutput() const;

    FileName parentDir() const;

    bool operator==(const FileName &other) const;
    bool operator!=(const FileName &other) const;
    bool operator<(const FileName &other) const;
    bool operator<=(const FileName &other) const;
    bool operator>(const FileName &other) const;
    bool operator>=(const FileName &other) const;

    bool isChildOf(const FileName &s) const;
    bool isChildOf(const QDir &dir) const;
    bool endsWith(const QString &s) const;

    Utils::FileName relativeChildPath(const FileName &parent) const;
    Utils::FileName &appendPath(const QString &s);
    Utils::FileName &append(const QString &str);
    Utils::FileName &append(QChar str);

    using QString::size;
    using QString::count;
    using QString::length;
    using QString::isEmpty;
    using QString::isNull;
    using QString::clear;
private:
    FileName(const QString &string);
};

class TOTEM_UTILS_EXPORT Path : public XmlSerializable
{
public:
	DECLARE_SERIALIZABLE(Path, Path)
	virtual void serialize(XmlSerializer& s) const;
	virtual void deserialize(XmlDeserializer& s) ;
    QString m_path; //!< 路径
    bool bRecursion;//!< 是否递归该路径
};



class TOTEM_UTILS_EXPORT FileUtils
{
public:
    static bool removeRecursively(const QString &filePath, QString *error = 0);

    static QString resolveSymlinks(const QString &path, const int &iMaxLevel = 16);
    static bool copyRecursively(const QString &srcFilePath,
                         const QString &tgtFilePath, QString *error = 0);
    static bool isFileNewerThan(const QString &filePath,
                            const QDateTime &timeStamp);

};

class TOTEM_UTILS_EXPORT FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    FileSaverBase();
    virtual ~FileSaverBase();

    QString fileName() const { return m_fileName; }
    bool hasError() const { return m_hasError; }
    QString errorString() const { return m_errorString; }
    virtual bool finalize();
    bool finalize(QString *errStr);
    bool finalize(QWidget *parent);

    bool write(const char *data, int len);
    bool write(const QByteArray &bytes);
    bool setResult(QTextStream *stream);
    bool setResult(QDataStream *stream);
    bool setResult(QXmlStreamWriter *stream);
    bool setResult(bool ok);

protected:
    QFile *m_file;
    QString m_fileName;
    QString m_errorString;
    bool m_hasError;

private:
    //禁用拷贝构造函数
    Q_DISABLE_COPY(FileSaverBase)
};

class TOTEM_UTILS_EXPORT FileSaver : public FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils)
public:
    explicit FileSaver(const QString &filename, QIODevice::OpenMode mode = QIODevice::NotOpen); // QIODevice::WriteOnly is implicit

    virtual bool finalize();
    using FileSaverBase::finalize;
    QFile *file() { return m_file; }

private:
    bool m_isSafe;
};


class TOTEM_UTILS_EXPORT TempFileSaver : public FileSaverBase
{
    Q_DECLARE_TR_FUNCTIONS(Utils::FileUtils) // sic!
public:
    explicit TempFileSaver(const QString &templ = QString());
    ~TempFileSaver();

    QTemporaryFile *file() { return reinterpret_cast<QTemporaryFile *>(m_file); }

    void setAutoRemove(bool on) { m_autoRemove = on; }

private:
    bool m_autoRemove;
};
}


#endif // FILEUTILS_H
