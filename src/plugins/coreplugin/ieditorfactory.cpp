#include "ieditorfactory.h"
#include "idocument.h"
#include "ieditor.h"
#include "editormanager.h"
namespace Core{

IDocument *IEditorFactory::open(const QString &fileName)
{
    Core::IEditor *iface = Core::EditorManager::openEditor(fileName, id());
    return iface ? iface->document() : 0;
}

}
