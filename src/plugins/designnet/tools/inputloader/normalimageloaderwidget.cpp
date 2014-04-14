#include "StdAfx.h"
#include "normalimageloaderwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QUrl>
#include "graphicsnormalimageloader.h"


using namespace DesignNet;
namespace InputLoader {
NormalImageLoaderWidget::NormalImageLoaderWidget(GraphicsNormalImageLoader *processor)
	: ProcessorFrontWidget(processor), m_processor(processor)
{
	setMinimumSize(200, 200);
	setAcceptDrops(true);
	setAutoFillBackground(true);
	m_text = tr("<drag content>");
}

NormalImageLoaderWidget::~NormalImageLoaderWidget()
{

}

void NormalImageLoaderWidget::paintEvent(QPaintEvent * event)
{
	QPainter p(this);
	QRect rc = this->rect();
	rc.adjust(1, 1, -1, -1);
	p.setPen(Qt::darkCyan);
	p.drawRect(rc);
	p.fillRect(rc, Qt::darkGray);
	p.setPen(Qt::white);
	p.drawText(this->rect(), Qt::AlignCenter, m_text);
}

void NormalImageLoaderWidget::dragEnterEvent(QDragEnterEvent *event)
{
	m_text = tr("<drop content>");
	update();
	event->acceptProposedAction();
}

void NormalImageLoaderWidget::dragMoveEvent(QDragMoveEvent *event)
{
	event->acceptProposedAction();
}

void NormalImageLoaderWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	m_text = tr("<drag content>");
	update();
}

void NormalImageLoaderWidget::dropEvent(QDropEvent *event)
{
	const QMimeData *mimeData = event->mimeData();

	if (mimeData->hasImage())
	{
	}
	else if (mimeData->hasText())
	{
		m_text = mimeData->text();
	}
	else if (mimeData->hasUrls())
	{
		QList<QUrl> urlList = mimeData->urls();
		QString text;
		for (int i = 0; i < urlList.size() && i < 32; ++i)
		{
			if (urlList.at(i).isLocalFile())
			{
				text = urlList.at(i).toLocalFile();
				m_processor->setPath(text);
				m_processor->start();
				break;
			}
		}
		m_text += text;
	}
	else
	{
		m_text = tr("Error");
	}
	event->acceptProposedAction();
	update();
}
}
