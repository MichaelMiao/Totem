#ifndef INPUTLOADER_H
#define INPUTLOADER_H

#include "inputloader_global.h"
#include "extensionsystem/iplugin.h"


namespace InputLoader{

class ImageFolderLoader;
class GraphicsNormalImageLoader;
class LabelLoader;
class InputLoaderPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:

    explicit InputLoaderPlugin(QObject *parent = 0) { }
	virtual ~InputLoaderPlugin() { }
	virtual bool initialize(const QStringList &arguments, QString *errorString);
    virtual void extensionsInitialized();

protected:

	ImageFolderLoader*			m_imageFolderLoader;
	GraphicsNormalImageLoader*	m_imageLoader;
	LabelLoader*				m_labelLoader;
};
}

#endif // INPUTLOADER_H
