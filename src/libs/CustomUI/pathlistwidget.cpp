#include "pathlistwidget.h"
#include "filterlineedit.h"

#include <QHBoxLayout>
#include <QFileDialog>
#include <QTableView>
#include <QToolButton>
#include <QGridLayout>
#include <QFileDialog>
namespace CustomUI{

PathListWidget::PathListWidget(QDir::Filters filters,
                               const QStringList &nameFilters,
                               bool bSingleSelection,
                               QWidget *parent) :
    QWidget(parent),
    m_filters(filters),
    m_nameFilters(nameFilters),
    m_buttonAddFolder(0),
    m_buttonAddFile(0),
    m_bSingleSelection(bSingleSelection)
{
    m_gridLayout = new QGridLayout();
    setLayout(m_gridLayout);
    QHBoxLayout *hLayout = new QHBoxLayout();

    m_pathLineEdit = new FancyLineEdit(this);

    QIcon icon(QLatin1String(":/customui/media/delete.png"));

    m_pathLineEdit->setButtonPixmap(FancyLineEdit::Right, icon.pixmap(16));
    m_pathLineEdit->setButtonVisible(FancyLineEdit::Right, true);
    m_pathLineEdit->setAutoHideButton(FancyLineEdit::Right, true);
    m_pathLineEdit->setPlaceholderText(tr("Select Path"));
    m_pathLineEdit->setButtonToolTip(FancyLineEdit::Right, tr("Delete Path"));
    hLayout->addWidget(m_pathLineEdit);
    connect(m_pathLineEdit, SIGNAL(buttonClicked(CustomUI::FancyLineEdit::Side)), this, SLOT(deletePath()));

    if(filters & QDir::Files)
    {
        m_buttonAddFile = new QToolButton();
        m_buttonAddFile->setIcon(QIcon(":/customui/media/add_file.png"));
        m_buttonAddFile->setAutoRaise(true);
        hLayout->addWidget(m_buttonAddFile);
        connect(m_buttonAddFile, SIGNAL(clicked()), this, SLOT(addPath()));
    }
    if(filters & QDir::Dirs)
    {
        m_buttonAddFolder = new QToolButton();
        m_buttonAddFolder->setIcon(QIcon(":/customui/media/add_folder.png"));
        m_buttonAddFolder->setAutoRaise(true);
        hLayout->addWidget(m_buttonAddFolder);
        connect(m_buttonAddFolder, SIGNAL(clicked()), this, SLOT(addPath()));
    }

    m_gridLayout->addLayout(hLayout, 0, 0, 1, 3);

    m_model = new PathListModel(m_bSingleSelection, this);
    m_tableView = new QTableView(this);
    m_tableView->setModel(m_model);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QTableView::SingleSelection);
    connect(m_tableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(selectChanged(QModelIndex)));
	if(m_bSingleSelection)
	{
		m_tableView->setVisible(false);
	}
    m_gridLayout->addWidget(m_tableView, 1, 0, 3, 3);
}

void PathListWidget::setFilters(QDir::Filters filters)
{
    m_filters = filters;
}

void PathListWidget::setNameFilters(const QStringList &filterList)
{
    m_nameFilters = filterList;
}

QList<Path> PathListWidget::paths() const
{
    return m_model->m_paths;
}

void PathListWidget::addPath()
{
    if(m_filters & QDir::Writable)
    {
        if(sender() == m_buttonAddFile)
        {
            QString filename = QFileDialog::getSaveFileName(this, tr("Save"),
                                         QDir::currentPath(),
                                         m_nameFilters.join(";;"));
			if(!filename.isEmpty())
            {
				m_model->addPath(filename);
				m_pathLineEdit->setText(filename);
			}
        }
    }
    else
    {
        if(sender() == m_buttonAddFile)
        {
            if(m_bSingleSelection)
            {
                QString filename = QFileDialog::getOpenFileName(this, tr("Open File"),
                                         QDir::currentPath(),
                                         m_nameFilters.join(";;"));
				if(!filename.isEmpty())
				{
					m_model->addPath(filename);
					m_pathLineEdit->setText(filename);
				}
            }
            else
            {
                QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open Files"),
                                         QDir::currentPath(),
                                         m_nameFilters.join(";;"));
                foreach(QString filename, filenames)
                {
					m_model->addPath(filename);
				}
            }
        }
        else if(sender() == m_buttonAddFolder)
        {
            QString directoryName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                         QDir::currentPath());
			if(!directoryName.isEmpty())
			{
				m_model->addPath(directoryName);
				m_pathLineEdit->setText(directoryName);
			}
           
        }
    }
    emit dataChanged();
    m_tableView->resizeColumnsToContents();
}

void PathListWidget::deletePath()
{
    QModelIndex index = m_tableView->selectionModel()->currentIndex();
    if(index.isValid())
    {
        m_model->removeRow(index.row());
        if(m_model->rowCount() > 0)
            m_tableView->selectRow(0);
        else
        {
            m_pathLineEdit->setButtonVisible(FancyLineEdit::Right, false);
            m_pathLineEdit->clear();
        }
        emit dataChanged();
    }
}

void PathListWidget::selectChanged(QModelIndex index)
{
    if(index.isValid())
    {
        QModelIndex tempIndex = m_model->index(index.row(), 0);
        m_pathLineEdit->setText(tempIndex.data().toString());
        m_pathLineEdit->setButtonVisible(FancyLineEdit::Right, true);
    }
}
}
