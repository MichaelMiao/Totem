#include "editordata.h"
namespace DesignNet{

EditorData::EditorData(QObject *parent) :
    QObject(parent)
{
    m_editor    = 0;
    m_view      = 0;
}
}
