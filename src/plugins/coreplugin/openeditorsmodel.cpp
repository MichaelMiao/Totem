#include "openeditorsmodel.h"
#include "ieditor.h"
#include "idocument.h"

#include <QIcon>
#include <QList>
#include <QFileInfo>
#include <QDir>
namespace Core{
class OpenEditorsModelPrivate
{
public:

    OpenEditorsModelPrivate();

    const QIcon m_lockedIcon;
    const QIcon m_unlockedIcon;
    QList<OpenEditorsModel::Entry> m_editors;

};

OpenEditorsModelPrivate::OpenEditorsModelPrivate()
{

}

//------------------------------------------------------

OpenEditorsModel::Entry::Entry() : editor(0)
{

}

QString OpenEditorsModel::Entry::fileName() const
{
    return editor ? editor->document()->fileName() : m_fileName;
}

QString OpenEditorsModel::Entry::displayName() const
{
    return editor ? editor->displayName() : m_displayName;
}

Id OpenEditorsModel::Entry::id() const
{
    return editor ? editor->id() : m_id;
}

OpenEditorsModel::OpenEditorsModel(QObject *parent) :
    QAbstractItemModel(parent), d(new OpenEditorsModelPrivate)
{

}

OpenEditorsModel::~OpenEditorsModel()
{
    delete d;
}

int OpenEditorsModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant OpenEditorsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (index.column() != 0 && role < Qt::UserRole))
        return QVariant();
    Entry e = d->m_editors.at(index.row());
    switch (role) {
    case Qt::DisplayRole:
        return (e.editor && e.editor->document()->isModified())
                ? e.displayName() + QLatin1Char('*')
                : e.displayName();
    case Qt::DecorationRole:
    {
        bool showLock = false;
        if (e.editor)
        {
            showLock = e.editor->document()->fileName().isEmpty()
                    ? false
                    : e.editor->document()->isFileReadOnly();
        }
        else
        {
            showLock = !QFileInfo(e.m_fileName).isWritable();
        }
        return showLock ? d->m_lockedIcon : QIcon();
    }
    case Qt::ToolTipRole:
        return e.fileName().isEmpty()
                ? e.displayName()
                : QDir::toNativeSeparators(e.fileName());
    case Qt::UserRole:
        return qVariantFromValue(e.editor);
    case Qt::UserRole + 1:
        return e.fileName();
    case Qt::UserRole + 2:
        return QVariant::fromValue(e.editor ? Core::Id(e.editor->id()) : e.id());
    default:
        return QVariant();
    }
    return QVariant();
}

int OpenEditorsModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return d->m_editors.count();
    return 0;
}

QModelIndex OpenEditorsModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (column < 0 || column > 1 || row < 0 || row >= d->m_editors.count())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex OpenEditorsModel::indexOf(IEditor *editor) const
{
    int idx = findEditor(editor);
    return createIndex(idx, 0);
}

QList<IEditor *> OpenEditorsModel::editors() const
{
    QList<IEditor *> result;
    foreach (const Entry &entry, d->m_editors)
        if (entry.editor)
            result += entry.editor;
    return result;
}

IEditor *OpenEditorsModel::editorAt(int row) const
{
    return d->m_editors.at(row).editor;
}

QList<OpenEditorsModel::Entry> OpenEditorsModel::restoredEditors() const
{
    QList<Entry> result;
    for (int i = d->m_editors.count()-1; i >= 0; --i)
    {
        if (!d->m_editors.at(i).editor)
        {
            result.append(d->m_editors.at(i));
        }
    }
    return result;
}

QList<OpenEditorsModel::Entry> OpenEditorsModel::entries() const
{
    return d->m_editors;
}

void OpenEditorsModel::addEditor(IEditor *editor)
{
    if (!editor)
        return;

    Entry entry;
    entry.editor = editor;
    addEntry(entry);
}

void OpenEditorsModel::addRestoredEditor(const QString &fileName, const QString &displayName, const Id &id)
{
    Entry entry;
    entry.m_fileName = fileName;
    entry.m_displayName = displayName;
    entry.m_id = id;
    addEntry(entry);
}

QModelIndex OpenEditorsModel::firstRestoredEditor() const
{
    for (int i = 0; i < d->m_editors.count(); ++i)
        if (!d->m_editors.at(i).editor)
            return createIndex(i, 0);
    return QModelIndex();
}

void OpenEditorsModel::removeEditor(IEditor *editor)
{
    removeEditor(findEditor(editor));
}

void OpenEditorsModel::removeEditor(const QModelIndex &index)
{
    removeEditor(index.row());
}

void OpenEditorsModel::removeEditor(const QString &fileName)
{
    removeEditor(findFileName(fileName));
}
//将所有的Editor关闭
void OpenEditorsModel::removeAllRestoredEditors()
{
    for (int i = d->m_editors.count()-1; i >= 0; --i)
    {
        //移出model中IEditor为空的项
        if (!d->m_editors.at(i).editor)
        {
            beginRemoveRows(QModelIndex(), i, i);
            d->m_editors.removeAt(i);
            endRemoveRows();
        }
    }
}

void OpenEditorsModel::emitDataChanged(IEditor *editor)
{
    int idx = findEditor(editor);
    if (idx < 0)
        return;
    QModelIndex mindex = index(idx, 0);
    emit dataChanged(mindex, mindex);
}

void OpenEditorsModel::itemChanged()
{
    emitDataChanged(qobject_cast<IEditor*>(sender()));
}

void OpenEditorsModel::addEntry(const OpenEditorsModel::Entry &entry)
{
    QString fileName = entry.fileName();

    int previousIndex = findFileName(fileName);
    if (previousIndex >= 0)
    {
        if (entry.editor && d->m_editors.at(previousIndex).editor == 0)
        {
            d->m_editors[previousIndex] = entry;
            connect(entry.editor, SIGNAL(changed()), this, SLOT(itemChanged()));
        }
        return;
    }

    int index;
    QString displayName = entry.displayName();
    for (index = 0; index < d->m_editors.count(); ++index) {
        if (displayName < d->m_editors.at(index).displayName())
            break;
    }

    beginInsertRows(QModelIndex(), index, index);
    d->m_editors.insert(index, entry);
    if (entry.editor)
        connect(entry.editor, SIGNAL(changed()), this, SLOT(itemChanged()));
    endInsertRows();
}

int OpenEditorsModel::findEditor(IEditor *editor) const
{
    for (int i = 0; i < d->m_editors.count(); ++i)
        if (d->m_editors.at(i).editor == editor)
            return i;
    return -1;
}

int OpenEditorsModel::findFileName(const QString &filename) const
{
    if (filename.isEmpty())
        return -1;
    for (int i = 0; i < d->m_editors.count(); ++i)
    {
        if (d->m_editors.at(i).fileName() == filename)
            return i;
    }
    return -1;
}

void OpenEditorsModel::removeEditor(int idx)
{
    if (idx < 0)
        return;
    IEditor *editor= d->m_editors.at(idx).editor;
    beginRemoveRows(QModelIndex(), idx, idx);
    d->m_editors.removeAt(idx);
    endRemoveRows();
    if (editor)
        disconnect(editor, SIGNAL(changed()), this, SLOT(itemChanged()));
}



}
