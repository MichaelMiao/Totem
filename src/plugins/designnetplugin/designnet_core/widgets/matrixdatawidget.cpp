#include "matrixdatawidget.h"
#include "../data/matrixdata.h"
#include "datadetailwidget.h"
#include "designnetconstants.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QGraphicsScene>
#include <QToolButton>
namespace DesignNet{

class MatrixDataWidgetPrivate{
public:
	MatrixDataWidgetPrivate(MatrixDataWidget *widget);
	~MatrixDataWidgetPrivate();
	QTableView *	m_tableView;
	MatrixModel*	m_matrixModel;
	DataDetailWidget* m_detailWidget;
	QGraphicsProxyWidget *m_proxyWidget;
	MatrixDataWidget *q;
};

MatrixDataWidgetPrivate::MatrixDataWidgetPrivate( MatrixDataWidget *widget )
	: q(widget)
{
	/// 设置弹出详细预览窗口
	m_detailWidget = new DataDetailWidget;
	QVBoxLayout *layout = new QVBoxLayout(m_detailWidget);
	m_detailWidget->setLayout(layout);
	m_proxyWidget = widget->scene()->addWidget(m_detailWidget, Qt::Tool);
	if (m_proxyWidget)
	{
		m_proxyWidget->setVisible(false);
	}

	m_tableView = new QTableView(m_detailWidget);
	layout->addWidget(m_tableView);
	m_matrixModel = new MatrixModel(widget);
	m_tableView->setModel(m_matrixModel);
}

MatrixDataWidgetPrivate::~MatrixDataWidgetPrivate()
{
	delete m_detailWidget;
}

MatrixDataWidget::MatrixDataWidget(IData *data, QGraphicsItem *parent, Qt::WindowFlags wFlags)
	: IDataWidget(data, true, parent, wFlags),
	d(new MatrixDataWidgetPrivate(this))
{
	setAttribute( Qt::WA_TranslucentBackground);
	setAutoFillBackground (false);
}

MatrixDataWidget::~MatrixDataWidget()
{

}

QRectF MatrixDataWidget::boundingRect() const
{
	return QRectF(0, 0, 100, 100);
}

QSizeF MatrixDataWidget::sizeHint( Qt::SizeHint which, const QSizeF & constraint /*= QSizeF()*/ ) const
{
	return QSizeF(100, 100);
}

void MatrixDataWidget::onUpdate()
{
	d->m_matrixModel->updateData();
}

void MatrixDataWidget::onShowDetail()
{
	d->m_proxyWidget->setVisible(true);
	d->m_proxyWidget->setZValue(DesignNet::Constants::ZValue_GraphicsBlock_Emphasize);
}

void MatrixDataWidget::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget )
{
	IDataWidget::paint(painter, option, widget);
}

int MatrixModel::rowCount( const QModelIndex & parent /*= QModelIndex() */ ) const
{
	const cv::Mat &mat = matrix();
	return mat.rows;
}

int MatrixModel::columnCount( const QModelIndex &/*parent*/ ) const
{
	cv::Mat &mat = matrix();
	return mat.cols;
}

QVariant MatrixModel::data( const QModelIndex & index, int role /*= Qt::DisplayRole */ ) const
{
	if(index.isValid() && role == Qt::DisplayRole)
	{
		int r = index.row();
		int c = index.column();
		const cv::Mat &mat = matrix();
		if(r >= mat.rows || c >= mat.cols || 
			(mat.depth() != CV_8U && mat.depth() != CV_32F)
			|| mat.channels() > 3)
		{
			return QVariant();
		}
		int deta = 0;/// 偏移、像素中的第几个通道
		switch(mat.type()){
		case CV_8UC3:
			deta = c % 3;
			return mat.at<cv::Vec3b>(r, c)[deta];
		case CV_8UC2:
			deta = c % 2;
			return mat.at<cv::Vec2b>(r, c)[deta];
		case CV_8UC1:
			return mat.at<uchar>(r, c);
		case CV_32FC1:
			return mat.at<float>(r, c);
		case CV_32FC2:
			deta = c%2;
			return mat.at<cv::Vec2b>(r, c)[deta];
		case CV_32FC3:
			return mat.at<cv::Vec3f>(r, c)[deta];
		}

		return QVariant();
	}
	return QVariant();
}

MatrixModel::MatrixModel( MatrixDataWidget *widget )
	: QAbstractTableModel(widget),
	m_widget(widget)
{
	
}

cv::Mat & MatrixModel::matrix() const
{
	MatrixData *data = qobject_cast<MatrixData*>(m_widget->m_data);
	if(data)
	{
		return data->getMatrix();
	}
	return cv::Mat();
}

void MatrixModel::updateData()
{
	reset();
}


}
