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

#ifndef DOCUMENTMANAGER_H
#define DOCUMENTMANAGER_H

#include "id.h"
#include <QObject>
#include <QPair>
#include <QList>
QT_BEGIN_NAMESPACE
class QMainWindow;
QT_END_NAMESPACE

namespace Core{

class IDocument;
class IContext;
namespace Internal{

class DocumentManagerPrivate;
}
class CORE_EXPORT DocumentManager : public QObject
{
    Q_OBJECT
public:
    enum FixMode {
        ResolveLinks,//解析快捷链接
        KeepLinks//保持快捷链接
    };
    typedef QPair<QString, Id> RecentFile;

    explicit DocumentManager(QMainWindow *mw);
    virtual ~DocumentManager();
    static DocumentManager *instance();

    static void addDocuments(const QList<IDocument *> &documents, bool addWatcher = true);
    static void addDocument(IDocument *document, bool addWatcher = true);
    static bool removeDocument(IDocument *document);
    static QList<IDocument *> modifiedDocuments();

    static void renamedFile(const QString &from, const QString &to);

    static void expectFileChange(const QString &fileName);
    static void unexpectFileChange(const QString &fileName);

    // recent files
    static void addToRecentFiles(const QString &fileName, const Id &editorId = Id());
    Q_SLOT void clearRecentFiles();
    static QList<RecentFile> recentFiles();

    static void saveSettings();
	static void readSettings();

    // current file
    static void setCurrentFile(const QString &filePath);
    static QString currentFile();

    // helper methods
    static QString fixFileName(const QString &fileName, FixMode fixmode);

    static bool saveDocument(IDocument *document, const QString &fileName = QString(), bool *isReadOnly = 0);

    static QStringList getOpenFileNames(const QString &filters,
                                 const QString path = QString(),
                                 QString *selectedFilter = 0);
    static QString getSaveFileName(const QString &title, const QString &pathIn,
                            const QString &filter = QString(), QString *selectedFilter = 0);
    static QString getSaveFileNameWithExtension(const QString &title, const QString &pathIn,
                                         const QString &filter);
    static QString getSaveAsFileName(IDocument *document, const QString &filter = QString(),
                              QString *selectedFilter = 0);

    static QList<IDocument *> saveModifiedDocumentsSilently(const QList<IDocument *> &documents, bool *cancelled = 0);
    static QList<IDocument *> saveModifiedDocuments(const QList<IDocument *> &documents,
                                     bool *cancelled = 0,
                                     const QString &message = QString(),
                                     const QString &alwaysSaveMessage = QString(),
                                     bool *alwaysSave = 0);

    static QString fileDialogLastVisitedDirectory();
    static void setFileDialogLastVisitedDirectory(const QString &directory);

    static QString fileDialogInitialDirectory();

    enum ReadOnlyAction { RO_Cancel, RO_MakeWriteable, RO_SaveAs };
    static ReadOnlyAction promptReadOnlyFile(const QString &fileName,
                                             QWidget *parent,
                                             bool displaySaveAsButton = false);



signals:
    void currentFileChanged(const QString &filePath);
    /* Used to notify e.g. the code model to update the given files. Does *not*
       lead to any editors to reload or any other editor manager actions. */
    void filesChangedInternally(const QStringList &files);
    /// emitted if all documents changed their name e.g. due to the file changing on disk
    void allDocumentsRenamed(const QString &from, const QString &to);
    /// emitted if one document changed its name e.g. due to save as
    void documentRenamed(Core::IDocument *document, const QString &from, const QString &to);


public slots:
    void mainWindowActivated();
    void checkForReload();
    void checkForNewFileName();
    void documentDestroyed(QObject *obj);
    void fileNameChanged(const QString &oldName, const QString &newName);
    void changedFile(const QString &fileName);
    void syncWithEditor(Core::IContext *context);

private:
//    Internal::DocumentManagerPrivate *d;

};
}

Q_DECLARE_METATYPE(Core::DocumentManager::RecentFile)

#endif // DOCUMENTMANAGER_H
