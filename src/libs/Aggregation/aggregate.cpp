#include "aggregate.h"

#include <QWriteLocker>
#include <QDebug>

/*!
    \namespace Aggregation
    \brief 命名空间Aggregation将相关的component组合到一起

    Aggregation 将components组合到一起，并通过qobject_cast进行转换。所有的component在Aggregation析构时删掉。
*/

/*!
    \class Aggregation::Aggregate
    \mainclass
    \threadsafe

    \brief components可以组合到一起，就像Union类型一样.

    Aggregate是Components的集合，以暴露出每一个component的属性和方法。
    \list
    \o 可以使用qobject_cast将Aggregate转换成指定类型 (使用 \a query and \a query_all 函数).
    \o 所有的component是耦合在一起的，一个delete，所有的都会delete
    \endlist
    Components需要时QObject的子类。

    你可以使用Aggregate来模仿多继承。假设我们有以下类定义。
    \code
        using namespace Aggregation;
        class MyInterface : public QObject { ........ };
        class MyInterfaceEx : public QObject { ........ };
        [...]
        MyInterface *object = new MyInterface; // this is single inheritance
    \endcode
    query方法可以像qobject_cast一样使用。
    \code
        Q_ASSERT(query<MyInterface>(object) == object);
        Q_ASSERT(query<MyInterfaceEx>(object) == 0);
    \endcode
    如果想让object用作MyInterfaceEx来使用，但是又不用多继承的话，我们就可以使用Aggregate
    \code
        MyInterfaceEx *objectEx = new MyInterfaceEx;
        Aggregate *aggregate = new Aggregate;
        aggregate->add(object);
        aggregate->add(objectEx);
    \endcode
    如果我们将两个或多个QObject的子类添加到Aggregate当中后，我们就可以通过其中的任何一个Object来
    获得其他的QObject，也就是别的component。
    \code
        Q_ASSERT(query<MyInterface>(object) == object);
        Q_ASSERT(query<MyInterfaceEx>(object) == objectEx);
        Q_ASSERT(query<MyInterface>(objectEx) == object);
        Q_ASSERT(query<MyInterfaceEx>(objectEx) == objectEx);
    \endcode
    以下代码将会删除所有的三个对象：object，objectEx和aggregate。
    \code
        delete objectEx;
        // or delete object;
        // or delete aggregate;
    \endcode

    使用Aggregation不用使用qobject_cast，但是可以使用Aggregation::query来替代。
*/

/*!
    \fn T *Aggregate::component()

    模板函数，如果存在，返回指定类型的component的模板函数。如果存在多个该类型的component，将会返回随机的一个。
    因为使用的是QHash。
    \sa Aggregate::components()
    \sa Aggregate::add()
*/

/*!
    \fn QList<T *> Aggregate::components()

    模板函数，返回所有指定类型的components。
    \sa Aggregate::component()
    \sa Aggregate::add()
*/

/*!
    \fn T *Aggregation::query<T *>(Aggregate *obj)
    \internal

    返回Aggregate *obj中的T类型的数据。
*/

/*!
    \fn QList<T *> Aggregation::query_all<T *>(Aggregate *obj)
    \internal

    返回所有Aggregate *\a obj 中的T类型的数据。
*/

/*!
    \relates Aggregation::Aggregate
    \fn T *Aggregation::query<T *>(QObject *obj)

    动态转换，如果\a obj 就是T类型的数据，就直接转换并返回，否则将会检查所有的components，返回检查到的第一个T类型的数据，
    如果没有T类型的数据，返回空。
    \sa Aggregate::component()
*/

/*!
    \relates Aggregation::Aggregate
    \fn QList<T *> Aggregation::query_all<T *>(QObject *obj)

    动态转换，如果\a obj 就是T类型的数据，就直接转换并返回，否则将会检查所有的components，返回检查到的所有T类型的数据列表，
    如果没有T类型的数据，返回空。

    \sa Aggregate::components()
*/

using namespace Aggregation;

/*!
    \fn Aggregate *Aggregate::parentAggregate(QObject *obj)

    如果obj被放到Aggregate中，就返回该Aggregate，否则返回0。
*/
Aggregate *Aggregate::parentAggregate(QObject *obj)
{
//    QReadLocker locker(&lock());
    return aggregateMap().value(obj);
}

QHash<QObject *, Aggregate *> &Aggregate::aggregateMap()
{
    static QHash<QObject *, Aggregate *> map;
    return map;
}

/*!
    \fn QReadWriteLock &Aggregate::lock()
    \internal
*/
QReadWriteLock &Aggregate::lock()
{
    static QReadWriteLock lock;
    return lock;
}

/*!
    \fn Aggregate::Aggregate(QObject *parent)

    \a parent 传递到QObject
*/
Aggregate::Aggregate(bool autoDelete, QObject *parent)
    : QObject(parent),
      m_autoDelete(autoDelete)
{
    QWriteLocker locker(&lock());
    aggregateMap().insert(this, this);
}

/*!
    \fn Aggregate::~Aggregate()

    自动删除掉aggregate，并delete所有的components。
*/
Aggregate::~Aggregate()
{
    QWriteLocker locker(&lock());
    foreach (QObject *component, m_components)
    {
        if(m_autoDelete)// 说明自动删除，要断开连接，否则不用断开连接
        {
            disconnect(component, SIGNAL(destroyed(QObject*)), this, SLOT(deleteSelf(QObject*)));
        }
        aggregateMap().remove(component);
    }
    qDeleteAll(m_components);
    m_components.clear();
    aggregateMap().remove(this);
}

void Aggregate::deleteSelf(QObject *obj)
{
    {
        QWriteLocker locker(&lock());
        aggregateMap().remove(obj);
        m_components.removeAll(obj);
    }
    delete this;
}

/*!
    \fn void Aggregate::add(QObject *component)

    添加 \a component 到 aggregate中.
    不能添加别的Aggregate的component，也不能添加Aggregate类型的对象。

    \sa Aggregate::remove()
*/
void Aggregate::add(QObject *component)
{
    if (!component)
        return;
    {
        QWriteLocker locker(&lock());
        Aggregate *parentAggregation = aggregateMap().value(component);
        if (parentAggregation == this)
            return;
        if (parentAggregation)
        {
            qWarning() << "Cannot add a component that belongs to a different aggregate" << component;
            return;
        }
        m_components.append(component);
        if(m_autoDelete)/// 如果自动删除，就需要连接信号
            connect(component, SIGNAL(destroyed(QObject*)), this, SLOT(deleteSelf(QObject*)));
        aggregateMap().insert(component, this);
    }
    emit changed();
}

/*!
    \fn void Aggregate::remove(QObject *component)

    从Aggregate中移除\a component (断开了\a component 的destroyed信号）

    \sa Aggregate::add()
*/
void Aggregate::remove(QObject *component)
{
    if (!component)
        return;
    {
        QWriteLocker locker(&lock());
        aggregateMap().remove(component);
        m_components.removeAll(component);
        if(m_autoDelete)
            disconnect(component, SIGNAL(destroyed(QObject*)), this, SLOT(deleteSelf(QObject*)));
    }
    emit changed();
}
