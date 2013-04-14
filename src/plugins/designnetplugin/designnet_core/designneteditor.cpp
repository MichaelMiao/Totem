#include "designneteditor.h"
#include "designnetdocument.h"
#include "designnetconstants.h"

#include "designnetmainwindow.h"
#include "designview.h"
#include "designnetbase/designnetspace.h"
#include <QTextEdit>
#include <QFile>
#include <QFileInfo>
#include <QToolBar>
#include <QAction>
using namespace Core;
namespace DesignNet{
class DesignNetEditorPrivate
{
public:
    DesignNetEditorPrivate(QWidget *parent);
    QString displayName;
    DesignNetDocument *file;
    DesignNetSpace* m_space;
    QTextEdit   *   m_textEdit;
    DesignView  *   m_designView;
    QToolBar *      m_toolBar;
};

DesignNetEditorPrivate::DesignNetEditorPrivate(QWidget *parent)
{
    m_textEdit  = new QTextEdit(parent);
    m_space     = new DesignNetSpace(0, parent);
    m_designView = new DesignView(m_space, parent);

}

DesignNetEditor::DesignNetEditor(QWidget *parent)
    : d(new DesignNetEditorPrivate(parent))
{
    d->file = new DesignNetDocument(this);
    setWidget(d->m_textEdit);
    connect(d->file, SIGNAL(changed()), this, SIGNAL(changed()));
}

DesignView *DesignNetEditor::designNetWidget()
{
    return d->m_designView;
}

bool DesignNetEditor::createNew(const QString &contents)
{
    Q_UNUSED(contents)
    return false;
}

bool DesignNetEditor::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
    return d->file->open(errorString, fileName, realFileName);
}

Core::IDocument *DesignNetEditor::document()
{
    return d->file;
}

Core::Id DesignNetEditor::id() const
{
    return Core::Id(Constants::GRAPHICS_DESIGNNET_ID);
}

QString DesignNetEditor::displayName() const
{
    return d->file->fileName();
}

void DesignNetEditor::setDisplayName(const QString &title)
{
    d->displayName = title;
    emit changed();
}

QByteArray DesignNetEditor::saveState() const
{
    return QByteArray();
}

bool DesignNetEditor::restoreState(const QByteArray &state)
{
    return true;
}

bool DesignNetEditor::isTemporary() const
{
    return false;
}

QWidget *DesignNetEditor::toolBar()
{
    return 0;
}

Id DesignNetEditor::preferredModeType() const
{
    return DesignNet::Constants::DESIGNNET_MODETYPE;
}

DesignNetSpace *DesignNetEditor::designNetSpace() const
{
    return d->m_space;
}



}
