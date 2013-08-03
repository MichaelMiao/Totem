#ifndef EDITORTOOLBAR_H
#define EDITORTOOLBAR_H

#include "core_global.h"

#include <QIcon>
#include <QWidget>
#include "CustomUI/styledbar.h"
namespace Core{
class IEditor;

struct EditorToolBarPrivate;

class CORE_EXPORT EditorToolBar : public CustomUI::StyledBar
{
    Q_OBJECT
public:
    explicit EditorToolBar(QWidget *parent = 0);
    virtual ~EditorToolBar();

	enum ToolbarCreationFlags { FlagsNone = 0, FlagsStandalone = 1 };

	void setCreationFlags(ToolbarCreationFlags flag);

    void addEditor(IEditor *editor);
    void setCurrentEditor(IEditor *editor);
    void removeToolbarForEditor(IEditor *editor);
    void addCenterToolBar(QWidget *toolBar);
signals:
    void closeClicked();
public slots:
    void updateEditorStatus(IEditor *editor);
	void updateEditorListSelection(Core::IEditor *newSelection);
private slots:

    void closeEditor();
    void updateActionShortcuts();
    void checkEditorStatus();
private:
    void updateToolBar(QWidget *toolBar);

    EditorToolBarPrivate *d;

};
}

#endif // EDITORTOOLBAR_H
