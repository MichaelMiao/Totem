#ifndef MATRIXDATAWIDGET_H
#define MATRIXDATAWIDGET_H

#include "idatawidget.h"
#include <QObject>
#include <QAbstractTableModel>
#include <opencv2/core/core.hpp>
namespace DesignNet{
class MatrixDataWidget;
class MatrixModel : public QAbstractTableModel
{
	Q_OBJECT
public:

	MatrixModel(MatrixDataWidget *widget);
	int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	int columnCount(const QModelIndex &/*parent*/) const;
	QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	void updateData();

protected:

	cv::Mat matrix() const;
	mutable MatrixDataWidget *m_widget;
};

class MatrixDataWidgetPrivate;
class DESIGNNET_CORE_EXPORT MatrixDataWidget : public IDataWidget
{
	Q_OBJECT
	friend class MatrixModel;
public:
	MatrixDataWidget(IData *data, QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = Qt::FramelessWindowHint);
	~MatrixDataWidget();
	virtual QRectF boundingRect() const;
	virtual QSizeF sizeHint(Qt::SizeHint which,
		const QSizeF & constraint = QSizeF()) const;
	virtual void onUpdate();
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
	virtual void onShowDetail();
private:
	MatrixDataWidgetPrivate* d;
};
}

#endif // MATRIXDATAWIDGET_H
