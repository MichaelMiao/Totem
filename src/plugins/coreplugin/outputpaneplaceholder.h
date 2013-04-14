#ifndef OUTPUTPANEPLACEHOLDER_H
#define OUTPUTPANEPLACEHOLDER_H

#include "core_global.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QSplitter;
QT_END_NAMESPACE
namespace Core{

class IMode;

namespace Internal{
class OutputPaneManager;
}

struct OutputPanePlaceHolderPrivate;

class CORE_EXPORT OutputPanePlaceHolder : public QWidget
{
    Q_OBJECT
    friend class Core::Internal::OutputPaneManager;
public:
    explicit OutputPanePlaceHolder(Core::IMode *mode, QSplitter *parent = 0);
    ~OutputPanePlaceHolder();

    static OutputPanePlaceHolder *getCurrent();
    static bool isCurrentVisible();

    void unmaximize();
    bool isMaximized() const;
    void ensureSizeHintAsMinimum();

private slots:
    void currentModeChanged(Core::IMode *);
private:
    bool canMaximizeOrMinimize() const;
    void maximizeOrMinimize(bool maximize);

    OutputPanePlaceHolderPrivate *d;
};
}

#endif // OUTPUTPANEPLACEHOLDER_H
