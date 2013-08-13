#include "block.h"
using namespace Core;
namespace DesignNet{

Block::Block(Id id, QObject *parent)
    : m_id(id)
{

}

void Block::setCatagory(QString strCatagory)
{
    m_catagory = strCatagory;
}

void Block::setTitle(QString strTitle)
{
    m_title = strTitle;
}


Block::Block(QString strCatagory, QString strTitle, QObject *parent)
    : m_id(strCatagory + strTitle),
      m_catagory(strCatagory),
      m_title(strTitle)
{

}


}
