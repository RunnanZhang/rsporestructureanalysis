/**************************************************************************************************
*FILE: rstableeditor.cpp
*DESCRIPTION: 用于用户编辑数据库的窗口.
*MODULE NAME: 无
*
*CREATE DATE: 2013-10-08
*COPYRIGHT (C): 北京诺克斯达石油科技有限公司
*AUTHOR: 张润楠
*
*ENVIRONMENT: VS2008 Qt4.6.3
*
*CURRENT VERSION: GeoScope 2.9, 张润楠, 2013-10-08
*VERSION HISTORY:
*
*Note:
**************************************************************************************************/
#include "rstableeditor.h"
#include "rssqltablemodel.h"
#include <QtGui>
#include <QtSql>

RSTableEditor::RSTableEditor(RSSqlTableModel* model, QWidget *parent)
    : QDialog(parent)
{
	m_pModel = model;
    QGridLayout *pMainLayout = new QGridLayout(this);
	m_pView = new QTableView(this);
	m_pView->setModel(m_pModel);
    pMainLayout->addWidget(m_pView, 0, 0, 1, 1);

	QHBoxLayout *pButtonLayout = new QHBoxLayout(this);

	m_pSubmitBtn = new QPushButton(this);
	m_pSubmitBtn->setObjectName(QString::fromUtf8("m_pSubmitBtn"));
	//此为从ui自动生成代码，拷过来的。
	//m_pSubmitBtn->setText(QApplication::translate("RSTableEditor", "Submit", 0, QApplication::UnicodeUTF8));
	m_pSubmitBtn->setText(QString::fromLocal8Bit("提交"));
	pButtonLayout->addWidget(m_pSubmitBtn);

	m_pRrevertBtn = new QPushButton(this);
	m_pRrevertBtn->setObjectName(QString::fromUtf8("m_pRrevertBtn"));
	//m_pRrevertBtn->setText(QApplication::translate("RSTableEditor", "Revert", 0, QApplication::UnicodeUTF8));
	m_pRrevertBtn->setText(QString::fromLocal8Bit("回退"));
	pButtonLayout->addWidget(m_pRrevertBtn);

	m_pInsertBtn = new QPushButton(this);
	m_pInsertBtn->setObjectName(QString::fromUtf8("m_pInsertBtn"));
	//m_pInsertBtn->setText(QApplication::translate("RSTableEditor", "Insert", 0, QApplication::UnicodeUTF8));
	m_pInsertBtn->setText(QString::fromLocal8Bit("插入"));
	pButtonLayout->addWidget(m_pInsertBtn);

	m_pDeleteBtn = new QPushButton(this);
	m_pDeleteBtn->setObjectName(QString::fromUtf8("m_pDeleteBtn"));
	//m_pDeleteBtn->setText(QApplication::translate("RSTableEditor", "Delete", 0, QApplication::UnicodeUTF8));
	m_pDeleteBtn->setText(QString::fromLocal8Bit("删除"));
	pButtonLayout->addWidget(m_pDeleteBtn);

	m_pQuitBtn = new QPushButton(this);
	m_pQuitBtn->setObjectName(QString::fromUtf8("m_pQuitBtn"));
	//m_pQuitBtn->setText(QApplication::translate("RSTableEditor", "Quit", 0, QApplication::UnicodeUTF8));
	m_pQuitBtn->setText(QString::fromLocal8Bit("退出"));
	pButtonLayout->addWidget(m_pQuitBtn);

	m_pSubmitBtn->setDefault(true);
	connect(m_pSubmitBtn, SIGNAL(clicked()), this, SLOT(submit()));
	connect(m_pInsertBtn, SIGNAL(clicked()), this, SLOT(insertRow()));
	connect(m_pDeleteBtn, SIGNAL(clicked()), this, SLOT(deleteRow()));
	connect(m_pRrevertBtn, SIGNAL(clicked()), m_pModel, SLOT(revertAll()));
	connect(m_pQuitBtn, SIGNAL(clicked()), this, SLOT(close()));

	pMainLayout->addLayout(pButtonLayout, 1, 0, 1, 1);
    setLayout(pMainLayout);
	resize(1000, 460);
    setWindowTitle(QString::fromLocal8Bit("缓存编辑窗"));
}

void RSTableEditor::submit()
{
    m_pModel->database().transaction();
    if (m_pModel->submitAll())
	{
        m_pModel->database().commit();
    } 
	else
	{
        m_pModel->database().rollback();
        QMessageBox::warning(this, tr("Cached Table"),
                             tr("The database reported an error: %1")
                             .arg(m_pModel->lastError().text()));
    }
}

void RSTableEditor::insertRow()
{
	int nCurRow = m_pView->currentIndex().row();
	if (nCurRow == -1)
	{
		nCurRow = m_pModel->rowCount()-1;
	}
	m_pModel->insertRow(nCurRow+1);
	m_pModel->setData(m_pModel->index(nCurRow+1, 0), nCurRow);
	m_pModel->submitAll();
}

void RSTableEditor::deleteRow()
{
	QItemSelectionModel *selections = m_pView->selectionModel();
	QModelIndexList selecteds = selections->selectedIndexes();
	foreach(QModelIndex index, selecteds)
	{
		m_pModel->removeRow(index.row());
	}
	int nRet = QMessageBox::warning(this,"Delete", QString("Do you wang to delete these selected rows now ? " )+ 
													"Delete will directly submit to the database.",
												  QMessageBox::Ok | QMessageBox::Cancel);
	if (nRet == QMessageBox::Ok)
	{
		m_pModel->submitAll();
	}
	else
	{
		m_pModel->revertAll();
	}
}