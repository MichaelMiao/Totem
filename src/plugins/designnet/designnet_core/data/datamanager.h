#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include "designnet_core_global.h"
QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE
namespace DesignNet{
class DataManagerPrivate;
class IDataWidget;
class IData;
/*!
 * \brief 数据管理类，提供指定数据类型的Widget
 * \todo 数据管理类和属性管理类可以抽象成一个AbastractManager,因为他们功能相似
 */

class DESIGNNET_CORE_EXPORT DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = 0);
    ~DataManager();
    virtual IDataWidget *createWidget(IData *data, QGraphicsItem *parent = 0);

    static DataManager* instance();
signals:

public slots:
    void objectAdded(QObject* obj);
protected:
    static DataManager* m_instance;
    DataManagerPrivate* d;
};
}

#endif // DATAMANAGER_H
