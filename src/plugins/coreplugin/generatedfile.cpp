/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/
#include "stdafx.h"
#include "generatedfile.h"
#include <QCoreApplication>
#include <QDir>
#include <QString>
#include "utils/fileutils.h"



namespace Core{

class GeneratedFilePrivate : public QSharedData
{
public:

    GeneratedFilePrivate() : binary(false) {}
    explicit GeneratedFilePrivate(const QString &p);
    QString path;
    QByteArray contents;
    Id editorId;
    bool binary;
    GeneratedFile::Attributes attributes;
};

GeneratedFilePrivate::GeneratedFilePrivate(const QString &p) :
    path(QDir::cleanPath(p)),
    binary(false),
    attributes(0)
{
}

GeneratedFile::GeneratedFile() :
    m_d(new GeneratedFilePrivate)
{
}

GeneratedFile::GeneratedFile(const QString &p) :
    m_d(new GeneratedFilePrivate(p))
{
}

GeneratedFile::GeneratedFile(const GeneratedFile &rhs) :
    m_d(rhs.m_d)
{
}

GeneratedFile &GeneratedFile::operator=(const GeneratedFile &rhs)
{
    if (this != &rhs)
        m_d.operator=(rhs.m_d);
    return *this;
}

GeneratedFile::~GeneratedFile()
{
}

QString GeneratedFile::path() const
{
    return m_d->path;
}

void GeneratedFile::setPath(const QString &p)
{
    m_d->path = QDir::cleanPath(p);
}

QString GeneratedFile::contents() const
{
    return QString::fromUtf8(m_d->contents);
}

void GeneratedFile::setContents(const QString &c)
{
    m_d->contents = c.toUtf8();
}

QByteArray GeneratedFile::binaryContents() const
{
    return m_d->contents;
}

void GeneratedFile::setBinaryContents(const QByteArray &c)
{
    m_d->contents = c;
}

bool GeneratedFile::isBinary() const
{
    return m_d->binary;
}

void GeneratedFile::setBinary(bool b)
{
    m_d->binary = b;
}

Id GeneratedFile::editorId() const
{
    return m_d->editorId;
}

void GeneratedFile::setEditorId(const Id &id)
{
    m_d->editorId = id;
}

bool GeneratedFile::write(QString *errorMessage) const
{
    // Ensure the directory
    const QFileInfo info(m_d->path);
    const QDir dir = info.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.absolutePath())) {
            *errorMessage = QCoreApplication::translate("BaseFileWizard", "Unable to create the directory %1.").arg(
                        QDir::toNativeSeparators(dir.absolutePath()));
            return false;
        }
    }

    // Write out
    QIODevice::OpenMode flags = QIODevice::WriteOnly|QIODevice::Truncate;
    if (!isBinary())
        flags |= QIODevice::Text;

    Utils::FileSaver saver(m_d->path, flags);
    saver.write(m_d->contents);
    return saver.finalize(errorMessage);
}

GeneratedFile::Attributes GeneratedFile::attributes() const
{
    return m_d->attributes;
}

void GeneratedFile::setAttributes(Attributes a)
{
    m_d->attributes = a;
}
}
