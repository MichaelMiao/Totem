#pragma once

#include "idocumentfactory.h"


namespace Core{

class IEditor;
class CORE_EXPORT IEditorFactory : public Core::IDocumentFactory
{
    Q_OBJECT

public:

    explicit IEditorFactory(QObject *parent = 0): IDocumentFactory(parent){}
    virtual ~IEditorFactory(){}

    virtual IEditor *createEditor(QWidget *parent) = 0;
    virtual IDocument *open(const QString &fileName);
};
}
