#include "designneteditor.h"
#include "designnetdocument.h"
#include "designnetconstants.h"
#include "designnetview.h"
#include "designnetbase/designnetspace.h"
#include "graphicsitem/processorgraphicsblock.h"
#include "Utils/XML/xmldeserializer.h"
#include <QTextEdit>
#include <QMessageBox>
#include <QToolBar>
#include <QAction>
namespace DesignNet{

class DesignNetEditorPrivate
{
public:
	DesignNetEditorPrivate();
	~DesignNetEditorPrivate();
	DesignNetDocument *m_file;
	QTextEdit   *   m_textEdit;
	DesignNetView*	m_designNetView;
	QToolBar*		m_toolBar;
};

DesignNetEditorPrivate::DesignNetEditorPrivate()
{
	m_file			= 0;
	m_textEdit		= new QTextEdit;
	m_designNetView = new DesignNetView(0);
}

DesignNetEditorPrivate::~DesignNetEditorPrivate()
{
}

DesignNetEditor::DesignNetEditor(QObject *parent)
	: Core::IEditor(parent),
	d(new DesignNetEditorPrivate())
{
	setWidget(d->m_textEdit);
	
	d->m_file = new DesignNetDocument(this);
	d->m_designNetView->setDesignNetSpace(d->m_file->designNetSpace());
	d->m_toolBar = new QToolBar(tr("Build"), d->m_designNetView);

	QAction *pRunAction = new QAction(this);
	QIcon icon(":/media/start.png");
	pRunAction->setIcon(icon);
	d->m_toolBar->addAction(pRunAction);

	connect(d->m_file, SIGNAL(changed()), this, SIGNAL(changed()));
	connect(d->m_file, SIGNAL(deserialized(Utils::XmlDeserializer &)), 
		this, SLOT(onDeserialized(Utils::XmlDeserializer &)));
	connect(d->m_file, SIGNAL(serialized(Utils::XmlSerializer &)), 
		this, SLOT(onSerialized(Utils::XmlSerializer &)));

	createCommand();
}

DesignNetEditor::~DesignNetEditor()
{
	delete d;
}

bool DesignNetEditor::createNew( const QString &contents /*= QString()*/ )
{
	return true;
}

bool DesignNetEditor::open( QString *errorString, const QString &fileName, const QString &realFileName )
{
	bool bret = d->m_file->open(errorString, fileName, realFileName);
	return bret;
}

Core::IDocument * DesignNetEditor::document()
{
	return d->m_file;
}

Core::Id DesignNetEditor::id() const
{
	return DesignNet::Constants::GRAPHICS_DESIGNNET_ID;
}

QString DesignNetEditor::displayName() const
{
	return d->m_file->fileName();
}

void DesignNetEditor::setDisplayName( const QString &title )
{

}

QByteArray DesignNetEditor::saveState() const
{
	return QByteArray();
}

bool DesignNetEditor::restoreState( const QByteArray &state )
{
	return true;
}

bool DesignNetEditor::isTemporary() const
{
	return false;
}

QWidget * DesignNetEditor::toolBar()
{
	return d->m_toolBar;
}

Core::Id DesignNetEditor::preferredModeType() const
{
	return DesignNet::Constants::DESIGNNET_MODETYPE;
}

DesignNetView * DesignNetEditor::designNetView()
{
	return d->m_designNetView;
}

void DesignNetEditor::onDeserialized( Utils::XmlDeserializer &x )
{
	d->m_designNetView->deserialize(x);
}

void DesignNetEditor::onSerialized( Utils::XmlSerializer &s )
{
	d->m_designNetView->serialize(s);
}

void DesignNetEditor::createCommand()
{

}

void DesignNetEditor::run()
{
	d->m_file->designNetSpace()->setDataReady(true);
}

}
