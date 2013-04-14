#ifndef DESIGNNETEDITOR_H
#define DESIGNNETEDITOR_H
#include "coreplugin/ieditor.h"
#include "coreplugin/idocument.h"
#include <QObject>
namespace DesignNet{
class DesignNetEditorPrivate;
class DesignView;
class DesignNetSpace;
class DesignNetEditor : public Core::IEditor
{
    Q_OBJECT
public:
    DesignNetEditor(QWidget *parent);
    DesignView *designNetWidget();
    bool createNew(const QString &contents = QString());
    bool open(QString *errorString,
              const QString &fileName,
              const QString &realFileName);
    Core::IDocument *document();
    Core::Id id() const;
    QString displayName() const;
    void setDisplayName(const QString &title);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

    bool isTemporary() const;

    QWidget *toolBar();
    Core::Id preferredModeType() const;
    DesignNetSpace *designNetSpace() const;
public slots:

private:
    DesignNetEditorPrivate *d;
};
}//namespace DesignNet

#endif // DESIGNNETEDITOR_H
