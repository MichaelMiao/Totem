#include "idatawidget.h"
#include <QGraphicsScene>
#include <QHideEvent>
#include <QLabel>
#include <QPainter>
#include <QShowEvent>
#include "GraphicsUI/graphicsautoshowhideitem.h"


namespace DesignNet {

const char DETAILICON[] = ":/media/detail48.png";
IDataWidget::IDataWidget(IData *data, bool bDetailButton, QGraphicsItem *parent, Qt::WindowFlags wFlags) :
    QGraphicsProxyWidget(parent, wFlags),
		m_data(data),
		m_bDetailButton(bDetailButton)
{
    m_detailButton = new GraphicsUI::GraphicsAutoShowHideItem(this);
	
	m_detailButton->animateShow(bDetailButton);
	m_detailButton->setSize(QSize(32, 32));
	m_detailButton->setPos(boundingRect().right() - 32, boundingRect().top());
	m_detailButton->setPixmap(QPixmap(":/media/detail48.png"));
	connect(m_detailButton, SIGNAL(clicked()), this, SLOT(showDetail()));
}

QRectF IDataWidget::boundingRect() const
{
	if(m_bDetailButton)
		return m_detailButton->boundingRect().adjusted(-2, -2, 2, 2);
    return QRectF(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
}

void IDataWidget::updateData()
{
    onUpdate();
}

QSizeF IDataWidget::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    Q_UNUSED(which)
    Q_UNUSED(constraint)
    return boundingRect().size();
}

bool IDataWidget::isValid() const
{
	if(!m_data || !m_data->isValid())
		return false;
	return true;
}

void IDataWidget::showDetail()
{
	onShowDetail();
}

void IDataWidget::hideEvent( QHideEvent * event )
{
	closeConnection();
}

void IDataWidget::showEvent( QShowEvent * event )
{
	initConnection();
	onUpdate();
	update();
}

void IDataWidget::initConnection()
{
	connect(m_data, SIGNAL(dataChanged()), this ,SLOT(updateData()), Qt::QueuedConnection);
}

void IDataWidget::closeConnection()
{
	disconnect(m_data, SIGNAL(dataChanged()), this ,SLOT(updateData()));
}


}
