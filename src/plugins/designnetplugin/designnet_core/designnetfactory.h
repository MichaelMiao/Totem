#ifndef DESIGNNETFACTORY_H
#define DESIGNNETFACTORY_H

#include "coreplugin/ieditorfactory.h"

namespace DesignNet{
class DesignNetFactoryPrivate;
class DesignNetFactory : public Core::IEditorFactory
{
    Q_OBJECT
public:
    explicit DesignNetFactory(QObject *parent = 0);
    ~DesignNetFactory();
    Core::IEditor *createEditor(QWidget *parent);


    QStringList suffixTypes() const;

    Core::Id id() const;
    QString displayName() const;
signals:

public slots:
private:
    DesignNetFactoryPrivate *d;
};
}

#endif // DESIGNNETFACTORY_H
