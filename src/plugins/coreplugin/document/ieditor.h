#pragma once
#include <QMetaType>
#include "../Common/icontext.h"
#include "../Common/id.h"
#include "../core_global.h"


namespace Core{
class IDocument;

class CORE_EXPORT IEditor : public IContext
{
    Q_OBJECT

public:

    IEditor(QObject *parent = 0) : IContext(parent) {}
    virtual ~IEditor() {}

    virtual bool createNew(const QString &contents = QString()) = 0;
    virtual bool open(QString *errorString, const QString &fileName, const QString &realFileName) = 0;

    virtual IDocument *document() = 0;
    virtual Core::Id id() const = 0;
    virtual QString displayName() const = 0;
    virtual void setDisplayName(const QString &title) = 0;

    virtual QByteArray saveState() const = 0;
    virtual bool restoreState(const QByteArray &state) = 0;

    virtual bool isTemporary() const = 0;

    virtual QWidget *toolBar() = 0;

    virtual Id preferredModeType() const { return Id(); }

signals:

    void changed();
};

}//namespace Core
Q_DECLARE_METATYPE(Core::IEditor*)
