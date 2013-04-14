#ifndef INPUTLOADERPLUGIN_H
#define INPUTLOADERPLUGIN_H
#include "extensionsystem/iplugin.h"

namespace InputLoader{
class GraphicsNormalImageLoader;
class ImageFolderLoader;
class InputLoaderPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
public:
    explicit InputLoaderPlugin(QObject *parent = 0);
    virtual ~InputLoaderPlugin();
    virtual bool initialize(const QStringList &arguments, QString *errorString);
    virtual void extensionsInitialized();
protected:
    GraphicsNormalImageLoader*	m_normalImageLoader;
	ImageFolderLoader*			m_imageFolderLoader;
};
}

#endif // INPUTLOADERPLUGIN_H
