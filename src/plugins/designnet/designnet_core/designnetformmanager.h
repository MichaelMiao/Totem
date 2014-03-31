#ifndef DESIGNNETFORMMANAGER_H
#define DESIGNNETFORMMANAGER_H

#include <QObject>
QT_BEGIN_NAMESPACE
class QToolBar;
class QAction;
QT_END_NAMESPACE

namespace Core{
class Context;
class ActionContainer;
class Id;
}
namespace DesignNet{
class DesignNetFormManagerPrivate;
class DesignNetEditor;
class DesignNetFormManager : public QObject
{
	Q_OBJECT

public:
	DesignNetFormManager();
	~DesignNetFormManager();

	bool startInit();		//!< 开始初始化
	static DesignNetFormManager *instance();
	static void Release();	//!< 释放内存，调用析构

	DesignNetEditor *createEditor(QWidget *parent);

	QToolBar *createEditorToolBar() const;
	void createControlToolBar();

public slots:

	void onRunDesignNet();
	void onDesignNetFinished();

protected:
	void setupActions();
	void addToolAction(QAction* pAction, const Core::Context& context, const Core::Id &id, 
		Core::ActionContainer *pContainer, const QString& keySequence);

private:
	DesignNetFormManagerPrivate *d;
	static DesignNetFormManager *m_instance;
};

}

#endif // DESIGNNETFORMMANAGER_H
