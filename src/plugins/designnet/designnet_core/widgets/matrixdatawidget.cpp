#include "matrixdatawidget.h"
#include "../data/matrixdata.h"
#include "datadetailwidget.h"
#include "designnetconstants.h"
#include "coreplugin/icore.h"
#include "coreplugin/mainwindow.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QPushButton>
#include <QGraphicsScene>
#include <QToolButton>
#include <QMessageBox>
namespace DesignNet{

class MatrixDataWidgetPrivate{
public:
	MatrixDataWidgetPrivate(MatrixDataWidget *widget);
	~MatrixDataWidgetPrivate();
	QTableView *	m_tableView;
	MatrixModel*	m_matrixModel;
	DataDetailWidget* m_detailWidget;
	MatrixDataWidget *q;
};

MatrixDataWidgetPrivate::MatrixDataWidgetPrivate( MatrixDataWidget *widget )
	: q(widget)
{
	/// 设置弹出详细预览窗口
	m_detailWidget = new DataDetailWidget(Core::ICore::mainWindow(), Qt::Tool);
	QVBoxLayout *layout = new QVBoxLayout(m_detailWidget);
	m_detailWidget->setLayout(layout);

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
	delete d;
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
	d->m_detailWidget->show();
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

cv::Mat MatrixModel::matrix() const
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
	beginResetModel();
	endResetModel();
}


}
