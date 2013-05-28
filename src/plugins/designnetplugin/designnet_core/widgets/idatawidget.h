#ifndef IDATATYPEWIDGET_H
#define IDATATYPEWIDGET_H

#include "coreplugin/id.h"
#include "designnet_core_global.h"
#include "data/idata.h"
#include <QGraphicsProxyWidget>
namespace GraphicsUI{
class GraphicsAutoShowHideItem;
}
namespace DesignNet{

/*!
 * \brief The IDataWidget class 为数据提供预览的窗口
 *
 *  数据预览窗口是添加到QGraphicsScene当中的，所以它的基类是QGraphicsWidget
 */
class DESIGNNET_CORE_EXPORT IDataWidget : public QGraphicsProxyWidget
{
    Q_OBJECT
    friend class IData;
public:
    enum {
        DEFAULT_WIDTH  = 100,
        DEFAULT_HEIGHT = 100
    };
    explicit IDataWidget(IData *data, bool bDetailButton = false,
		QGraphicsItem *parent = 0, 
		Qt::WindowFlags wFlags = Qt::FramelessWindowHint);
    virtual ~IDataWidget(){}
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeHint(Qt::SizeHint which,
                            const QSizeF & constraint = QSizeF()) const;
	virtual bool isValid()const;
	virtual void	hideEvent ( QHideEvent * event );
	virtual void	showEvent ( QShowEvent * event );
	void initConnection();
	void closeConnection();
signals:

public slots:
    void updateData();          //!< 根据数据变化更新窗口
	void showDetail();
protected:
	virtual void onShowDetail() = 0;//!< 显示详细信息
    virtual void onUpdate() = 0;    //!< 由子类来完成如何更新
    IData *m_data;
	bool m_bDetailButton;		//!< 是否使用detail按钮
	GraphicsUI::GraphicsAutoShowHideItem * m_detailButton;
};
}

#endif // IDATATYPEWIDGET_H
