#ifndef MINISPLITTER_H
#define MINISPLITTER_H

#include "core_global.h"

#include <QSplitter>

QT_BEGIN_NAMESPACE
class QSplitterHandle;
QT_END_NAMESPACE

namespace Core {

class CORE_EXPORT MiniSplitter : public QSplitter
{
public:


    MiniSplitter(QWidget *parent = 0);
    MiniSplitter(Qt::Orientation orientation);

protected:

    QSplitterHandle *createHandle();
};

} // namespace Core

#endif // MINISPLITTER_H
