#include "stdafx.h"
#include "designnetdocument.h"
#include <QFileInfo>
#include "Utils/XML/xmldeserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmlserializer.h"
#include "designnet_core_def.h"
#include "DesignNetEditor.h"


using namespace Core;

//----------------------------------------------
DesignNetDocument::DesignNetDocument(DesignNetEditor *parent)
    : IDocument(parent),
	m_pEditor(parent), m_bOpening(false), m_bModified(false)
{
}

DesignNetDocument::~DesignNetDocument()
{
}

QString DesignNetDocument::defaultPath() const
{
    return QString();
}

QString DesignNetDocument::suggestedFileName() const
{
    return QString();
}

QString DesignNetDocument::suffixType() const
{
    return DesignNet::Constants::NETEDITOR_FILETYPE;
}

bool DesignNetDocument::shouldAutoSave() const
{
    return true;
}

bool DesignNetDocument::isModified() const
{
	return m_bModified;
}

bool DesignNetDocument::isSaveAsAllowed() const
{
    return true;
}

void DesignNetDocument::rename(const QString &newName)
{
	QString oldFilename = m_fileName;
	m_fileName = newName;
	m_pEditor->setDisplayName(QFileInfo(m_fileName).fileName());
    emit fileNameChanged(oldFilename, newName);
    emit changed();
}

void DesignNetDocument::setModified(const bool &bModified)
{
    if(bModified == m_bModified)
        return;
	m_bModified = bModified;
    emit changed();
	
	emit titleChanged(bModified ? m_fileName + "*" : m_fileName);
}

bool DesignNetDocument::reload(QString *errorString, IDocument::ReloadFlag flag, IDocument::ChangeType type)
{
    if (flag == FlagIgnore)
        return true;
    if (type == TypePermissions)
    {
        emit changed();
        return true;
    }
	return m_pEditor->open(errorString, m_fileName, m_fileName);
}

bool DesignNetDocument::save(QString *errorString, const QString &fileName, bool autoSave)
{
// 	Utils::XmlSerializer x;
// 	x.serialize("DesignNetSpace", *(d->space));
// 	emit serialized(x);
// 	x.write(fileName);
	setModified(false);
    return true;
}

void DesignNetDocument::onModified()
{
	if (!m_bOpening)
		setModified(true);
}

bool DesignNetDocument::open( QString *errorString, const QString &fileName, const QString &realFileName )
{
	m_bOpening = true;
	bool bRet = IDocument::open(errorString, fileName, realFileName);
// 	Utils::XmlDeserializer deserializer(realFileName);
// 	deserializer.deserialize("DesignNetSpace", *(d->space));
// 	emit deserialized(deserializer);
	m_bOpening = false;
	return bRet;
}
