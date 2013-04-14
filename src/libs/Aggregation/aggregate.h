/*!
 * \file aggregate.h
 * \author Michael_BJFU
 * \ref 代码几乎没有进行修改，引用QtCreator源代码，完成特定功能
 * \brief
 *
 */
#ifndef QAGGREGATION_H
#define QAGGREGATION_H

#include "aggregation_global.h"

#include <QObject>
#include <QList>
#include <QHash>
#include <QReadWriteLock>
#include <QReadLocker>

namespace Aggregation {
/*!
 * \class Aggregate
 * \brief Aggregate 对象集合相当于Union类型
 */
class AGGREGATION_EXPORT Aggregate : public QObject
{
    Q_OBJECT

public:
    Aggregate(bool autoDelete = true, QObject *parent = 0);
    virtual ~Aggregate();

    void add(QObject *component);   //!< 添加QObject到m_components当中
    void remove(QObject *component);//!< 在m_components中移除制定QObject

    ///
    /// 得到列表中第一个指定类型的component，这里就是用qobject_cast遍历m_components（QList)列表
    /// 有互斥控制
    template <typename T> T *component() {
        QReadLocker(&lock());
        foreach (QObject *component, m_components) {
            if (T *result = qobject_cast<T *>(component))
                return result;
        }
        return (T *)0;
    }

    ///
    /// 返回列表中所有指定类型的component。
    /// 有互斥控制
    template <typename T> QList<T *> components() {
        QReadLocker(&lock());
        QList<T *> results;
        foreach (QObject *component, m_components) {
            if (T *result = qobject_cast<T *>(component)) {
                results << result;
            }
        }
        return results;
    }

    ///
    /// 返回obj所在的Aggregate（集合）
    static Aggregate *parentAggregate(QObject *obj);

    /// 访问锁
    static QReadWriteLock &lock();

signals:
    void changed();

private slots:
    void deleteSelf(QObject *obj);

private:
    static QHash<QObject *, Aggregate *> &aggregateMap();

    QList<QObject *> m_components;
    bool m_autoDelete;  //!< 当某一个component删除时，是否自动删除所以该集合中的所有对象
};

/*!
 * \fn query(Aggregate *obj)
 * 从全局模板函数中查询<em> Aggregate obj </em> 中类型为\a T 的component
 */
template <typename T> T *query(Aggregate *obj)
{
    if (!obj)
        return (T *)0;
    return obj->component<T>();
}

/*!
 * \fn T *query(QObject *obj)
 * 在\a obj 所在的集合中查找第一个为T类型的component
 */
template <typename T> T *query(QObject *obj)
{
    if (!obj)
        return (T *)0;
    T *result = qobject_cast<T *>(obj);
    if (!result) {
        QReadLocker(&lock());
        Aggregate *parentAggregation = Aggregate::parentAggregate(obj);
        result = (parentAggregation ? query<T>(parentAggregation) : 0);
    }
    return result;
}

/*!
 * 返回在\a obj 所在的集合中查找所有为T类型的component
 */
template <typename T> QList<T *> query_all(Aggregate *obj)
{
    if (!obj)
        return QList<T *>();
    return obj->template components<T>();
}

/*!
 *  返回\a obj 所在集合中所有的T类型的component
 *  \sa query_all(Aggregate *obj)
 */
template <typename T> QList<T *> query_all(QObject *obj)
{
    if (!obj)
        return QList<T *>();
    QReadLocker(&lock());
    Aggregate *parentAggregation = Aggregate::parentAggregate(obj);
    QList<T *> results;
    if (parentAggregation)
        results = query_all<T>(parentAggregation);
    else if (T *result = qobject_cast<T *>(obj))
        results.append(result);
    return results;
}

} // namespace Aggregation

#endif // QAGGREGATION_H
