#pragma once

#include <QObject>
#include "../core_global.h"



QT_BEGIN_NAMESPACE
class QStringList;
QT_END_NAMESPACE

namespace Core {

class IDocument;
class Id;

class CORE_EXPORT IDocumentFactory : public QObject
{
    Q_OBJECT

public:
    IDocumentFactory(QObject *parent = 0) : QObject(parent) {}
    virtual ~IDocumentFactory() {}
    virtual QStringList suffixTypes() const = 0;
    virtual Id id() const = 0;
    virtual QString displayName() const = 0;
    virtual IDocument *open(const QString &fileName) = 0;
};

}
