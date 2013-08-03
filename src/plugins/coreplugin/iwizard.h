#ifndef IWIZARD_H
#define IWIZARD_H

#include "core_global.h"
#include "id.h"
#include <QObject>
#include <QVariantMap>
QT_BEGIN_NAMESPACE
class QIcon;
QT_END_NAMESPACE

namespace Core{

class CORE_EXPORT IWizard : public QObject
{
    Q_OBJECT
public:
    enum WizardKind {
        FileWizard = 0x01
    };
    Q_DECLARE_FLAGS(WizardKinds, WizardKind)
	enum WizardFlag {
		PlatformIndependent = 0x01,
		ForceCapitalLetterForFileName = 0x02
	};
	Q_DECLARE_FLAGS(WizardFlags, WizardFlag)
    IWizard(QObject *parent = 0) : QObject(parent) {}
	virtual ~IWizard(){}
    virtual WizardKind kind() const = 0;
    virtual QIcon icon() const = 0;
    virtual QString description() const = 0;
    virtual QString displayName() const = 0;
    virtual QString id() const = 0;

    virtual QString category() const = 0;
	virtual QString displayCategory() const = 0;

    virtual QString descriptionImage() const = 0;
    virtual void runWizard(const QString &path, QWidget *parent,const QVariantMap &variables) = 0;

    static QList<IWizard*> allWizards();
    static QList<IWizard*> wizardsOfKind(WizardKind kind);

};
}

#endif // IWIZARD_H
