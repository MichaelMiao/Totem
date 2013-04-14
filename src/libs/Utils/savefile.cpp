#include "savefile.h"

#include "totemassert.h"
#include "fileutils.h"

namespace Utils {

SaveFile::SaveFile(const QString &filename) :
    m_finalFileName(filename), m_finalized(true), m_backup(false)
{
}

SaveFile::~SaveFile()
{
    TOTEM_ASSERT(m_finalized, rollback());
}

bool SaveFile::open(OpenMode flags)
{
    TOTEM_ASSERT(!m_finalFileName.isEmpty() && fileName().isEmpty(),
                 return false);

    QFile ofi(m_finalFileName);
    //如果文件存在，却不能以读写的方式打开，返回false
    if (ofi.exists() && !ofi.open(QIODevice::ReadWrite))
    {
        setErrorString(ofi.errorString());
        return false;
    }

    setAutoRemove(false);
    setFileTemplate(m_finalFileName);
    if (!QTemporaryFile::open(flags))
        return false;

    m_finalized = false; // 在最后需要清理
    if (ofi.exists())
        setPermissions(ofi.permissions()); // Ignore errors

    return true;
}

void SaveFile::rollback()
{
    remove();
    m_finalized = true;
}

bool SaveFile::commit()
{
    TOTEM_ASSERT(!m_finalized, return false);
    m_finalized = true;

    close();
    if (error() != NoError)
    {
        remove();
        return false;
    }
    //????????????????有疑问,作用是什么?????????????
    QString finalFileName = Utils::FileUtils::resolveSymlinks(m_finalFileName);
    QString bakname = finalFileName + QLatin1Char('~');
    QFile::remove(bakname); // Kill old backup
    QFile::rename(finalFileName, bakname); // Backup current file
    if (!rename(finalFileName))
    { // Replace current file
        QFile::rename(bakname, finalFileName); // Rollback to current file
        return false;
    }
    if (!m_backup)
        QFile::remove(bakname);

    return true;
}

} // namespace Utils