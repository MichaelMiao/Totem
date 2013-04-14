#ifndef EDITMODE_H
#define EDITMODE_H

#include <coreplugin/imode.h>
#include "core_global.h"

QT_BEGIN_NAMESPACE
class QSplitter;
class QVBoxLayout;
QT_END_NAMESPACE

namespace Core {

class EditorManager;

class CORE_EXPORT EditMode : public Core::IMode
{
    Q_OBJECT

public:
    EditMode();
    ~EditMode();

private slots:
    void grabEditorManager(Core::IMode *mode);

private:
    EditorManager *m_editorManager;
    QSplitter *m_splitter;
    QVBoxLayout *m_rightSplitWidgetLayout;
};


} // namespace Core

#endif // EDITMODE_H
