/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**************************************************************************/
#pragma once

#include <QObject>
#include "../core_global.h"


QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE
namespace Core {

class CORE_EXPORT IDocument : public QObject
{
    Q_OBJECT

public:

    enum ReloadSetting {
        AlwaysAsk = 0,
        ReloadUnmodified = 1,
        IgnoreAll = 2
    };

    //�ı�������;����1. �ڲ��ı� 2.�ļ����ⲿ�ı�
    enum ChangeTrigger {
        TriggerInternal,
        TriggerExternal
    };
    //�ļ��ı����ͣ����ݡ�����Ȩ�ޡ��Ƴ��ļ�
    enum ChangeType {
        TypeContents,
        TypePermissions,
        TypeRemoved
    };

    enum ReloadBehavior {
        BehaviorAsk,
        BehaviorSilent
    };

    enum ReloadFlag {
        FlagReload,
        FlagIgnore
    };

    IDocument(QObject *parent = 0);
    virtual ~IDocument();

    virtual bool save(QString *errorString, const QString &fileName = QString(), bool autoSave = false) = 0;
    virtual QString fileName() const;
    virtual bool isFileReadOnly() const;

    virtual bool open(QString *errorString,
                      const QString &fileName,
                      const QString &realFileName);
    virtual bool reload(QString *errorString);

    virtual QString defaultPath() const = 0;
    virtual QString suggestedFileName() const = 0;
    virtual QString suffixType() const = 0;

    virtual bool shouldAutoSave() const;
    virtual bool isModified() const = 0;
    virtual bool isSaveAsAllowed() const = 0;

    virtual ReloadBehavior reloadBehavior(ChangeTrigger state, ChangeType type) const;
    virtual bool reload(QString *errorString, ReloadFlag flag, ChangeType type) = 0;
    virtual void rename(const QString &newName) = 0;

    virtual void checkPermissions();

    bool autoSave(QString *errorString, const QString &fileName);
    void setRestoredFrom(const QString &name);
    void removeAutoSaveFile();

    bool hasWriteWarning() const { return m_hasWriteWarning; }
    void setWriteWarning(bool has) { m_hasWriteWarning = has; }

signals:

    void changed();

    void aboutToReload();
    void reloaded();
    void fileNameChanged(const QString &oldName, const QString &newName);
	void titleChanged(QString title);

protected:

	bool m_hasWriteWarning;
	bool m_restored;
	bool m_bReadOnly;
	QString m_autoSaveName;
	QString m_fileName;
    QTextDocument *m_document;
};
}
