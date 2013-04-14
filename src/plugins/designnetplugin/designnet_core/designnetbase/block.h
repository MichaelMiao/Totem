#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include "../designnet_core_global.h"
#include "coreplugin/id.h"
namespace DesignNet{

class DESIGNNET_CORE_EXPORT Block : public QObject
{
public:
    Block(QString strCatagory, QString strTitle, QObject *parent);

    Block(Core::Id id, QObject *parent);
    virtual ~Block(){}
    void setCatagory(QString strCatagory);
    void setTitle(QString strTitle);
    QString getCatagory(){return m_catagory;}
    QString getTitle(){return m_title;}
private:
    Core::Id m_id;
    QString m_catagory;
    QString m_title;
};
}

#endif // BLOCK_H
