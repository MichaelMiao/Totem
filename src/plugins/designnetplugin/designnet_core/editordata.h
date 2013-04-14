#ifndef EDITORDATA_H
#define EDITORDATA_H

#include <QObject>

namespace DesignNet{
class DesignNetEditor;
class DesignView;
class EditorData : public QObject
{
    Q_OBJECT
public:
    explicit EditorData(QObject *parent = 0);
    DesignNetEditor *m_editor; //IEditor派生类
    DesignView      *m_view;    //设计视图
};
}

#endif // EDITORDATA_H
