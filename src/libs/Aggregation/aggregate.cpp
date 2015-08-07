#include "aggregate.h"

#include <QWriteLocker>
#include <QDebug>

/*!
    \namespace Aggregation
    \brief �����ռ�Aggregation����ص�component��ϵ�һ��

    Aggregation ��components��ϵ�һ�𣬲�ͨ��qobject_cast����ת�������е�component��Aggregation����ʱɾ����
*/

/*!
    \class Aggregation::Aggregate
    \mainclass
    \threadsafe

    \brief components������ϵ�һ�𣬾���Union����һ��.

    Aggregate��Components�ļ��ϣ��Ա�¶��ÿһ��component�����Ժͷ�����
    \list
    \o ����ʹ��qobject_cast��Aggregateת����ָ������ (ʹ�� \a query and \a query_all ����).
    \o ���е�component�������һ��ģ�һ��delete�����еĶ���delete
    \endlist
    Components��ҪʱQObject�����ࡣ

    �����ʹ��Aggregate��ģ�¶�̳С����������������ඨ�塣
    \code
        using namespace Aggregation;
        class MyInterface : public QObject { ........ };
        class MyInterfaceEx : public QObject { ........ };
        [...]
        MyInterface *object = new MyInterface; // this is single inheritance
    \endcode
    query����������qobject_castһ��ʹ�á�
    \code
        Q_ASSERT(query<MyInterface>(object) == object);
        Q_ASSERT(query<MyInterfaceEx>(object) == 0);
    \endcode
    �������object����MyInterfaceEx��ʹ�ã������ֲ��ö�̳еĻ������ǾͿ���ʹ��Aggregate
    \code
        MyInterfaceEx *objectEx = new MyInterfaceEx;
        Aggregate *aggregate = new Aggregate;
        aggregate->add(object);
        aggregate->add(objectEx);
    \endcode
    ������ǽ���������QObject��������ӵ�Aggregate���к����ǾͿ���ͨ�����е��κ�һ��Object��
    ���������QObject��Ҳ���Ǳ��component��
    \code
        Q_ASSERT(query<MyInterface>(object) == object);
        Q_ASSERT(query<MyInterfaceEx>(object) == objectEx);
        Q_ASSERT(query<MyInterface>(objectEx) == object);
        Q_ASSERT(query<MyInterfaceEx>(objectEx) == objectEx);
    \endcode
    ���´��뽫��ɾ�����е���������object��objectEx��aggregate��
    \code
        delete objectEx;
        // or delete object;
        // or delete aggregate;
    \endcode

    ʹ��Aggregation����ʹ��qobject_cast�����ǿ���ʹ��Aggregation::query�������
*/

/*!
    \fn T *Aggregate::component()

    ģ�庯����������ڣ�����ָ�����͵�component��ģ�庯����������ڶ�������͵�component�����᷵�������һ����
    ��Ϊʹ�õ���QHash��
    \sa Aggregate::components()
    \sa Aggregate::add()
*/

/*!
    \fn QList<T *> Aggregate::components()

    ģ�庯������������ָ�����͵�components��
    \sa Aggregate::component()
    \sa Aggregate::add()
*/

/*!
    \fn T *Aggregation::query<T *>(Aggregate *obj)
    \internal

    ����Aggregate *obj�е�T���͵����ݡ�
*/

/*!
    \fn QList<T *> Aggregation::query_all<T *>(Aggregate *obj)
    \internal

    ��������Aggregate *\a obj �е�T���͵����ݡ�
*/

/*!
    \relates Aggregation::Aggregate
    \fn T *Aggregation::query<T *>(QObject *obj)

    ��̬ת�������\a obj ����T���͵����ݣ���ֱ��ת�������أ����򽫻������е�components�����ؼ�鵽�ĵ�һ��T���͵����ݣ�
    ���û��T���͵����ݣ����ؿա�
    \sa Aggregate::component()
*/

/*!
    \relates Aggregation::Aggregate
    \fn QList<T *> Aggregation::query_all<T *>(QObject *obj)

    ��̬ת�������\a obj ����T���͵����ݣ���ֱ��ת�������أ����򽫻������е�components�����ؼ�鵽������T���͵������б�
    ���û��T���͵����ݣ����ؿա�

    \sa Aggregate::components()
*/

using namespace Aggregation;

/*!
    \fn Aggregate *Aggregate::parentAggregate(QObject *obj)

    ���obj���ŵ�Aggregate�У��ͷ��ظ�Aggregate�����򷵻�0��
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

    \a parent ���ݵ�QObject
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

    �Զ�ɾ����aggregate����delete���е�components��
*/
Aggregate::~Aggregate()
{
    QWriteLocker locker(&lock());
    foreach (QObject *component, m_components)
    {
        if(m_autoDelete)// ˵���Զ�ɾ����Ҫ�Ͽ����ӣ������öϿ�����
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

    ��� \a component �� aggregate��.
    ������ӱ��Aggregate��component��Ҳ�������Aggregate���͵Ķ���

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
        if(m_autoDelete)/// ����Զ�ɾ��������Ҫ�����ź�
            connect(component, SIGNAL(destroyed(QObject*)), this, SLOT(deleteSelf(QObject*)));
        aggregateMap().insert(component, this);
    }
    emit changed();
}

/*!
    \fn void Aggregate::remove(QObject *component)

    ��Aggregate���Ƴ�\a component (�Ͽ���\a component ��destroyed�źţ�

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
