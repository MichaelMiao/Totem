#include "fileutils.h"
#include "totemassert.h"
#include "savefile.h"
#include "hostosinfo.h"

#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDateTime>
#include <QDataStream>
#include <QTextStream>
#include <QMessageBox>

namespace Utils{

bool FileUtils::removeRecursively(const QString &filePath, QString *error)
{
    QFileInfo fileInfo(filePath);//提供系统独立的文件信息，文件名、路径、权限、文件夹（或快捷方式）等。
    //如果不存在该路径，并且该路径也不是快捷方式，就不用删除了
    if (!fileInfo.exists() && !fileInfo.isSymLink())
        return true;
    //设置文件的访问权限（添加用户可写权限）
    QFile::setPermissions(filePath, fileInfo.permissions() | QFile::WriteUser);
    if (fileInfo.isDir())//指定路径是文件夹（或者通过快捷方式链接到一个文件夹）
    {
        QDir dir(filePath);
        dir = dir.canonicalPath();//去掉一些不必要的路径信息，返回绝对路径（返回快捷方式的目标路径）
        if (dir.isRoot())//不能删除根目录（C:，D:...）
        {
            if (error)
            {
                *error = QCoreApplication::translate("Utils::FileUtils",
                    "Refusing to remove root directory.");
            }
            return false;
        }
        //不能删除home（C:/Documents and Settings/Username）
        if (dir.path() == QDir::home().canonicalPath())
        {
            if (error)
            {
                *error = QCoreApplication::translate("Utils::FileUtils",
                    "Refusing to remove your home directory.");
            }
            return false;
        }
        //除了..和.之外的文件夹，文件、快捷方式、隐藏文件、系统文件，
        //这里返回的只有名字，不是完整路径
        QStringList fileNames = dir.entryList(QDir::Files
                                              | QDir::Hidden
                                              | QDir::System
                                              | QDir::Dirs
                                              | QDir::NoDotAndDotDot);

        foreach (const QString &fileName, fileNames)
        {
            //递归删除
            if (!removeRecursively(filePath + QLatin1Char('/') + fileName, error))
                return false;
        }
        //删除空的文件夹
        if (!QDir::root().rmdir(dir.path()))
        {
            if (error)
            {
                *error = QCoreApplication::translate("Utils::FileUtils", "Failed to remove directory '%1'.")
                        .arg(QDir::toNativeSeparators(filePath));
            }
            return false;
        }
    }
    else //说明是文件
    {
        if (!QFile::remove(filePath))
        {
            if (error)
            {
                *error = QCoreApplication::translate("Utils::FileUtils", "Failed to remove file '%1'.")
                        .arg(QDir::toNativeSeparators(filePath));
            }
            return false;
        }
    }
    return true;
}
/*!
    \fn resolveSymlinks()
    \brief 处理快捷方式，iMaxLevel指定最大链接次数
*/
QString FileUtils::resolveSymlinks(const QString &path, const int &iMaxLevel)
{
    QFileInfo f(path);
    int links = iMaxLevel;
    while (links-- && f.isSymLink())
        f.setFile(f.symLinkTarget());
    if (links <= 0)
        return QString();
    return f.filePath();
}

bool FileUtils::copyRecursively(const QString &srcFilePath, const QString &tgtFilePath, QString *error)
{
    QFileInfo srcFileInfo(srcFilePath);
    if (srcFileInfo.isDir())//是文件夹，应该先创建文件夹，然后拷贝里面文件
    {
        QDir targetDir(tgtFilePath);
        targetDir.cdUp();
        //目标文件夹不能存在
        if (!targetDir.mkdir(QFileInfo(tgtFilePath).fileName()))
        {
            if (error)
            {
                *error = QCoreApplication::translate("Utils::FileUtils", "Failed to create directory '%1'.")
                        .arg(QDir::toNativeSeparators(tgtFilePath));
                return false;
            }
        }
        QDir sourceDir(srcFilePath);
        QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
        //深度优先遍历即可
        foreach (const QString &fileName, fileNames)
        {
            const QString newSrcFilePath
                    = srcFilePath + QLatin1Char('/') + fileName;
            const QString newTgtFilePath
                    = tgtFilePath + QLatin1Char('/') + fileName;
            if (!copyRecursively(newSrcFilePath, newTgtFilePath, error))
                return false;
        }
    }
    else
    {
        if (!QFile::copy(srcFilePath, tgtFilePath))
        {
            if (error)
            {
                *error = QCoreApplication::translate("Utils::FileUtils", "Could not copy file '%1' to '%2'.")
                        .arg(QDir::toNativeSeparators(srcFilePath),
                             QDir::toNativeSeparators(tgtFilePath));
            }
            return false;
        }
    }
    return true;
}
/*!
 * \brief FileUtils::isFileNewerThan
 * 比较文件或文件夹是否比某个时间新，如果filePath是文件夹，那么它里面有一个文件比timeStamp新，
 * 就返回真
 */
bool FileUtils::isFileNewerThan(const QString &filePath, const QDateTime &timeStamp)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())//如果文件不存在，返回false
        return false;
    if (fileInfo.lastModified() >= timeStamp)//文件修改时间更新
        return true;
    if (fileInfo.isDir())
    {
        //这里不包括隐藏文件、快捷方式等系统文件
        const QStringList dirContents = QDir(filePath)
            .entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        foreach (const QString &curFileName, dirContents)
        {
            const QString curFilePath
                = filePath + QLatin1Char('/') + curFileName;
            if (isFileNewerThan(curFilePath, timeStamp))
                return true;
        }
    }
    return false;
}
void Path::serialize(XmlSerializer& s) const
{
	s.serialize("value", m_path);
	s.serialize("recursion", bRecursion);
}

void Path::deserialize(XmlDeserializer& s) 
{

}
//-  FileSaverBase ----------------------------

FileSaverBase::FileSaverBase()
    : m_hasError(false)
{
    m_file = 0;
}

FileSaverBase::~FileSaverBase()
{
    if(m_file)
        delete m_file;
}

bool FileSaverBase::finalize()
{
    if(m_file)
    {
        m_file->close();
        setResult(m_file->error() == QFile::NoError);

        delete m_file;
        m_file = 0;

    }
    return !m_hasError;
}

bool FileSaverBase::finalize(QString *errStr)
{
    if (finalize())
        return true;
    if (errStr)
        *errStr = errorString();
    return false;

}

bool FileSaverBase::finalize(QWidget *parent)
{
    if (finalize())
        return true;
    QMessageBox::critical(parent, tr("File Error"), errorString());
    return false;
}

bool FileSaverBase::write(const char *data, int len)
{
    if (m_hasError)
        return false;
    return setResult(m_file->write(data, len) == len);
}

bool FileSaverBase::write(const QByteArray &bytes)
{
    if (m_hasError)
        return false;
    return setResult(m_file->write(bytes) == bytes.count());
}

bool FileSaverBase::setResult(QTextStream *stream)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
    stream->flush();
    return setResult(stream->status() == QTextStream::Ok);
#else
    Q_UNUSED(stream)
    return true;
#endif

}

bool FileSaverBase::setResult(QDataStream *stream)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
    return setResult(stream->status() == QDataStream::Ok);
#else
    Q_UNUSED(stream)
    return true;
#endif
}

bool FileSaverBase::setResult(QXmlStreamWriter *stream)
{
#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
    return setResult(!stream->hasError());
#else
    Q_UNUSED(stream)
    return true;
#endif
}

bool FileSaverBase::setResult(bool ok)
{
    if (!ok && !m_hasError)
    {
        m_errorString = tr("Cannot write file %1. Disk full?").arg(
                QDir::toNativeSeparators(m_fileName));
        m_hasError = true;
    }
    return ok;

}

FileSaver::FileSaver(const QString &filename, QIODevice::OpenMode mode)
{
    m_fileName = filename;
    if (mode & (QIODevice::ReadOnly | QIODevice::Append))
    {
        m_file = new QFile(filename);
        m_isSafe = false;
    }
    else
    {
        m_file = new SaveFile(filename);
        m_isSafe = true;
    }
    if (!m_file->open(QIODevice::WriteOnly | mode))
    {
        QString err = QFile::exists(filename) ?
                tr("Cannot overwrite file %1: %2") : tr("Cannot create file %1: %2");
        m_errorString = err.arg(QDir::toNativeSeparators(filename), m_file->errorString());
        m_hasError = true;
    }
}

bool FileSaver::finalize()
{
    if (!m_isSafe)
        return FileSaverBase::finalize();

    SaveFile *sf = static_cast<SaveFile *>(m_file);
    if (m_hasError)
        sf->rollback();
    else
        setResult(sf->commit());
    delete sf;
    m_file = 0;
    return !m_hasError;

}

//-----------------------------------------------------------------
TempFileSaver::TempFileSaver(const QString &templ)
    : m_autoRemove(true)
{
    QTemporaryFile *tempFile = new QTemporaryFile();
    if (!templ.isEmpty())
        tempFile->setFileTemplate(templ);
    tempFile->setAutoRemove(false);
    if (!tempFile->open())
    {
          m_errorString = tr("Cannot create temporary file in %1: %2").arg(
                QDir::toNativeSeparators(QFileInfo(tempFile->fileTemplate()).absolutePath()),
                tempFile->errorString());
        m_hasError = true;
    }
    m_file = tempFile;
    m_fileName = tempFile->fileName();
}

TempFileSaver::~TempFileSaver()
{
    delete m_file;
    m_file = 0;
    if (m_autoRemove)
        QFile::remove(m_fileName);
}

/*! \class Utils::FileName

    \brief A light-weight convenience class for filenames

    On windows filenames are compared case insensitively.
*/

FileName::FileName()
    : QString()
{

}

/// Constructs a FileName from \a info
FileName::FileName(const QFileInfo &info)
    : QString(info.absoluteFilePath())
{
}

/// \returns a QFileInfo
QFileInfo FileName::toFileInfo() const
{
    return QFileInfo(*this);
}

/// \returns a QString for passing on to QString based APIs
QString FileName::toString() const
{
    return QString(*this);
}

/// \returns a QString to display to the user
/// Converts the separators to the native format
QString FileName::toUserOutput() const
{
    return QDir::toNativeSeparators(toString());
}

/// Find the parent directory of a given directory.

/// Returns an empty FileName if the current dirctory is already
/// a root level directory.

/// \returns \a FileName with the last segment removed.
FileName FileName::parentDir() const
{
    const QString basePath = toString();
    if (basePath.isEmpty())
        return FileName();

    const QDir base(basePath);
    if (base.isRoot())
        return FileName();

    const QString path = basePath + QLatin1String("/..");
    const QString parent = QDir::cleanPath(path);

    return FileName::fromString(parent);
}

/// Constructs a FileName from \a fileName
/// \a fileName is not checked for validity.
FileName FileName::fromString(const QString &filename)
{
    return FileName(filename);
}

/// Constructs a FileName from \a fileName
/// \a fileName is only passed through QDir::cleanPath
FileName FileName::fromUserInput(const QString &filename)
{
    return FileName(QDir::cleanPath(filename));
}

FileName::FileName(const QString &string)
    : QString(string)
{

}

bool FileName::operator==(const FileName &other) const
{
    return QString::compare(*this, other, HostOsInfo::fileNameCaseSensitivity()) == 0;
}

bool FileName::operator!=(const FileName &other) const
{
    return !(*this == other);
}

bool FileName::operator<(const FileName &other) const
{
    return QString::compare(*this, other, HostOsInfo::fileNameCaseSensitivity()) < 0;
}

bool FileName::operator<=(const FileName &other) const
{
    return QString::compare(*this, other, HostOsInfo::fileNameCaseSensitivity()) <= 0;
}

bool FileName::operator>(const FileName &other) const
{
    return other < *this;
}

bool FileName::operator>=(const FileName &other) const
{
    return other <= *this;
}

/// \returns whether FileName is a child of \a s
bool FileName::isChildOf(const FileName &s) const
{
    if (s.isEmpty())
        return false;
    if (!QString::startsWith(s, HostOsInfo::fileNameCaseSensitivity()))
        return false;
    if (size() <= s.size())
        return false;
    // s is root, '/' was already tested in startsWith
    if (s.QString::endsWith(QLatin1Char('/')))
        return true;
    // s is a directory, next character should be '/' (/tmpdir is NOT a child of /tmp)
    return at(s.size()) == QLatin1Char('/');
}

/// \overload
bool FileName::isChildOf(const QDir &dir) const
{
    return isChildOf(Utils::FileName::fromString(dir.absolutePath()));
}

/// \returns whether FileName endsWith \a s
bool FileName::endsWith(const QString &s) const
{
    return QString::endsWith(s, HostOsInfo::fileNameCaseSensitivity());
}

/// \returns the relativeChildPath of FileName to parent if FileName is a child of parent
/// \note returns a empty FileName if FileName is not a child of parent
/// That is, this never returns a path starting with "../"
FileName FileName::relativeChildPath(const FileName &parent) const
{
    if (!isChildOf(parent))
        return Utils::FileName();
    return FileName(QString::mid(parent.size() + 1, -1));
}

/// Appends \a s, ensuring a / between the parts
FileName &FileName::appendPath(const QString &s)
{
    if (!isEmpty() && !QString::endsWith(QLatin1Char('/')))
        append(QLatin1Char('/'));
    append(s);
    return *this;
}

FileName &FileName::append(const QString &str)
{
    QString::append(str);
    return *this;
}

FileName &FileName::append(QChar str)
{
    QString::append(str);
    return *this;
}
}