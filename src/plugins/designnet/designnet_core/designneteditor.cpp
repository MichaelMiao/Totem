#include "stdafx.h"
#include "DesignNetEditor.h"
#include "designnet_core_def.h"
#include "designnetmainwindow.h"
#include <QPushButton>

DesignNetEditor::DesignNetEditor(QObject* pObj, DesignNetMainWindow* pMainWindow) : Core::IEditor(pObj), m_pView(0), m_pDoc(0)
{
	m_pDoc = new DesignNetDocument(this);
	m_pView = new DesignNetView(pMainWindow);
	setWidget(m_pView);
}

bool DesignNetEditor::createNew(const QString &contents)
{
	return true;
}

bool DesignNetEditor::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
	return true;
}

Core::IDocument * DesignNetEditor::document()
{
	return m_pDoc;
}

Core::Id DesignNetEditor::id() const
{
	return DesignNet::Constants::NETEDITOR_ID;
}

QString DesignNetEditor::displayName() const
{
	return m_pDoc ? m_pDoc->fileName() : "";
}

void DesignNetEditor::setDisplayName(const QString &title)
{

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
	return true;
}
