#ifndef OPENEDITORSMODEL_H
#define OPENEDITORSMODEL_H

#include "core_global.h"
#include "id.h"
#include <QAbstractItemModel>
#include <QList>
namespace Core{

class OpenEditorsModelPrivate;
class IEditor;
class IDocument;

class CORE_EXPORT OpenEditorsModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit OpenEditorsModel(QObject *parent = 0);
    virtual ~OpenEditorsModel();
    struct CORE_EXPORT Entry {
        Entry();
        IEditor *editor;
        QString fileName() const;
        QString displayName() const;
        Id id() const;
        QString m_fileName;
        QString m_displayName;
        Id m_id;
    };
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex parent(const QModelIndex &/*index*/) const { return QModelIndex(); }
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex indexOf(IEditor *editor) const;

    QList<IEditor *> editors() const;
    IEditor *editorAt(int row) const;
    QList<Entry> restoredEditors() const;
    QList<Entry> entries() const;
    void addEditor(IEditor *editor);
    void addRestoredEditor(const QString &fileName, const QString &displayName, const Id &id);
    QModelIndex firstRestoredEditor() const;

    void removeEditor(IEditor *editor);
    void removeEditor(const QModelIndex &index);
    void removeEditor(const QString &fileName);
    void removeAllRestoredEditors();
    void emitDataChanged(IEditor *editor);

signals:

private slots:
    void itemChanged();
private:
    void addEntry(const Entry &entry);
    int findEditor(IEditor *editor) const;
    int findFileName(const QString &filename) const;
    void removeEditor(int idx);

    OpenEditorsModelPrivate *d;

};
}

#endif // OPENEDITORSMODEL_H
