/**************************************************************************************************
*FILE: rsporestructureanalysis.cpp
*DESCRIPTION: 相容性岩石物理建模主程序
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
#include "rsporestructureanalysis.h"
#include "rssqltablemodel.h"
#include "rstableeditor.h"
#include "rscomboboxdelegate.h"
#include <rscrossplotdlg.h>
#include <rscrossplotmainwin.h>
#include <rscpdataattr.h>
#include <rsuiproject.h>
#include <rsuibasic.h>
#include <uiselectobject.h>
#include <uiwell.h>
#include <q3listbox.h>
#include <QMessageBox>
#include <QCheckBox>
#include <QSqlError>
#include <QSqlQuery>
#include <qfile.h>
#include <qdir.h>
#include <QFileDialog>
#include <QDesktopWidget>
#include <QTextStream>

RSPoreStructureAnalysis::RSPoreStructureAnalysis(QWidget * parent, Qt::WindowFlags f)
:QMainWindow(parent, f), Ui_RSPoreStructureAnalysis()
{
	setupUi(this);
	init();
}

RSPoreStructureAnalysis::~RSPoreStructureAnalysis()
{
	destroy();
}

void RSPoreStructureAnalysis::init()
{
	m_pWell = NULL;
	m_pUIProject = NULL;
	m_pSingleWellManager = NULL;

	//Right-click menu for QTableView.
	wTableView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(wTableView, SIGNAL(customContextMenuRequested(const QPoint&)), 
		this, SLOT(slot_ShowMenu(const QPoint&)));
	m_pMenu = new QMenu(wTableView);
	m_pEditAction = m_pMenu->addAction(QString::fromLocal8Bit("编辑数据库..."));
	m_pEditAction->setShortcuts(QKeySequence::New/*(tr("Ctrl+E"))*/);
	m_pPopupAction = m_pMenu->addAction(QString::fromLocal8Bit("弹出所选区域..."));
	m_pPopupAction->setShortcuts(QKeySequence::Open/*(tr("Ctrl+E"))*/);
	connect(m_pEditAction, SIGNAL(triggered(bool)), this, SLOT(slot_EditAction()));
	connect(m_pPopupAction, SIGNAL(triggered(bool)), this, SLOT(slot_PopAction()));//use the same slot with m_pPopAction.

	//Right-click menu for QHeaderView.
	wTableView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(wTableView->horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)), 
		this, SLOT(slot_ShowHeaderMenu(const QPoint&)));
	m_pHeaderMenu = new QMenu(wTableView->horizontalHeader());
	m_pPopAction = m_pHeaderMenu->addAction(QString::fromLocal8Bit("弹出所选区域..."));
	connect(m_pPopAction, SIGNAL(triggered(bool)), this, SLOT(slot_PopAction()));
	m_pSortAction = m_pHeaderMenu->addAction(QString::fromLocal8Bit("排序"));
	connect(m_pSortAction, SIGNAL(triggered(bool)), this, SLOT(slot_SortAction()));
	m_nSortcol = 0;

	QDoubleValidator *pDv = new QDoubleValidator(this);
	wDminEdit->setValidator(pDv);
	wDrmEdit->setValidator(pDv);
	wKlpEdit->setValidator(pDv);
	wPorEdit->setValidator(pDv);
	wRrfEdit->setValidator(pDv);

	wTerminateBtn->setEnabled(false);
	m_pRSThread = NULL;
	m_nThreadFinish = 0;
}

void RSPoreStructureAnalysis::destroy()
{

}

void RSPoreStructureAnalysis::setLibName(const QString &strLibName)
{
	m_strLibraryName = strLibName;
}

void RSPoreStructureAnalysis::setProject(RSUIProject *p)
{
	Q_ASSERT(p != NULL);
	m_pUIProject = p;
	m_pSingleWellManager = m_pUIProject->child(Rs::type_Well_Well, true);
    //In this function we must use the m_pUIProject pointer,so the following init code should not called in init() function!
	createConnection();
	m_pModel = new RSSqlTableModel(0, this, m_db);//the 0th column will create checkbox.
	m_pModel->setTable("analysis");
	m_pModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
	m_pModel->select();
	m_pModel->initCheckVec();//init Vector<bool> which record the CheckBox column status.

	m_pModel->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("图片名称"));
	m_pModel->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("井名"));
	m_pModel->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("样品名称"));
	m_pModel->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("深度"));
	m_pModel->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("年代"));

	QObjectList childrenList = m_pSingleWellManager->children();
	QObjectList::const_iterator it;
	QStringList slWellName;
	RSUIBasic *pUI;
	for (it = childrenList.constBegin(); it != childrenList.constEnd(); ++it)
	{
		pUI = (RSUIBasic*)(*it);
		slWellName.append( (pUI->getIDComponent())->name() );
	}
	m_pDelegate = new RSComboBoxDelegate(slWellName, this);

	QModelIndex idx = m_pModel->index(1, 0);//test set the check box
	QVariant var = QVariant(Qt::Checked);
	m_pModel->setData(idx, var, Qt::CheckStateRole);
	var = m_pModel->data(idx, Qt::CheckStateRole);

	wTableView->setModel(m_pModel);
	wTableView->setItemDelegateForColumn(1, m_pDelegate);
}

void RSPoreStructureAnalysis::closeEvent(QCloseEvent * e)
{
	emit closeThread();
	emit closeLibrary(m_strLibraryName);
	e->accept();
}

void RSPoreStructureAnalysis::removeUIObject(RSUIBasic *pUI)
{
	if (m_pWell == pUI)
	{
		m_pWell = NULL;
		QMessageBox::warning(this, "Well Deleted !", 
			"The selected well has been deleted in porestructureanalysis, please select again !");
	}
}

bool RSPoreStructureAnalysis::createConnection()
{
	QString strPro = m_pUIProject->getIDComponent()->getDirectory();
	strPro += QDir::separator();
	strPro += "Window";
	strPro += QDir::separator();
	strPro += "poreAnalysis";
	strPro += ".db";
	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName(strPro);
	if (!m_db.open()) {
		QMessageBox::critical(0, qApp->tr("Cannot open database"),
			qApp->tr("Unable to establish a database connection.\n"
			"This example needs SQLite support. Please read "
			"the Qt SQL driver documentation for information how "
			"to build it.\n\n"
			"Click Cancel to exit."), QMessageBox::Cancel);
		return false;
	}
	QSqlQuery query(m_db);
	query.exec("create table analysis (PicName int primary key, "
		"WellName varchar(20), SampleNum int, "
		"Depth numeric(10,3), Era int, Lithology varchar(20), Vp real,"
		"Vs float, Density float, Porosity float, Permeability float,"
		"Oil float, Gas float, Water float,"
		"PoresNum int, Ratio float, Perimeter float, Area double)");
	//query.exec("insert into analysis values(101, 'Danny', 1,1,1,'1',1,1,1,1,1,1,1,2,2,2,1.123133,1)");
	return true;
}

void RSPoreStructureAnalysis::slot_WellSelectClicked()
{
	if(!m_pSingleWellManager) return;
	UIWell *pWellTemp = m_pWell;
	UISelectObject *pSelectObjDlg = new UISelectObject(this);
	pSelectObjDlg->setParent(m_pSingleWellManager);
	pSelectObjDlg->appendType(Rs::type_Well_Well);
	pSelectObjDlg->setSelectMode(Q3ListBox::Single);
	if(m_pWell)
		pSelectObjDlg->setInitSelectedObject((RSUIBasic **)&m_pWell, 1);
	pSelectObjDlg->initShow();
	if(pSelectObjDlg->exec() == QDialog::Accepted)
	{
		Q3PtrList<RSUIBasic> *pSelectList = pSelectObjDlg->getSelectedItem();
		m_pWell = (UIWell *)(pSelectList->first());
		if(pSelectList) delete pSelectList;
		wWellEdit->clear();
		if(m_pWell && m_pWell != pWellTemp)//m_pWell != pWellTemp Avoid multiple connect for the same well.
		{
			wWellEdit->setText((m_pWell->getIDComponent())->name());
			connect(m_pWell, SIGNAL(removeSelf(RSUIBasic *)),
				this, SLOT(removeUIObject(RSUIBasic *)));
		}
	}
	delete pSelectObjDlg;
}

/**************************************************************************************************
* Purpose: 选择图片，一张图片一个线程（可在Terminate Button随时终止）.
* Parameter: void
* Return: void
* Throws: NULL
* Note: 主程序删除线程优先选择Deletelater()，可保证退出消息循环后自动删除.
* Others:
**************************************************************************************************/
void RSPoreStructureAnalysis::slot_PictureBtnClicked()
{
	wReadInfoEdit->clear();
	bool bOk;
	if (wPorEdit->text().isEmpty() || wDrmEdit->text().isEmpty() ||
		wRrfEdit->text().isEmpty() || wKlpEdit->text().isEmpty() || wDminEdit->text().isEmpty())
	{
		wReadInfoEdit->append("Read parameter fail, please check again !");
		wReadInfoEdit->append("Reading picture abort.");
		return;
	}
	QDir dir;
	QString strPro;
	strPro = m_pUIProject->getIDComponent()->getDirectory();
	strPro += QDir::separator();
	strPro += "Window";
	strPro += QDir::separator();
	strPro += "RSPorestructureanalysis";
	wReadInfoEdit->append(QString::fromLocal8Bit("目标路径：") +strPro);
	bOk = dir.mkpath(strPro);
	if (!bOk)
	{
		wReadInfoEdit->append("Create path failed !");
		return;
	}
	strPro += QDir::separator();

	float fpor, fdrm, frrf, fklp, fdmin;
	fpor = wPorEdit->text().toFloat();
	fdrm = wDrmEdit->text().toFloat();
	frrf = wRrfEdit->text().toFloat();
	fklp = wKlpEdit->text().toFloat();
	fdmin = wDminEdit->text().toFloat();

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg *.bmp)"));
	dialog.setViewMode(QFileDialog::List);
	int nIndex = 0;
	QString strPicPath, strTemp;
	if (dialog.exec())
	{
		m_picNames = dialog.selectedFiles();
		if (m_picNames.isEmpty())
		{
			wReadInfoEdit->append("No picture was found, update the database failed !");
			return;
		}
		m_pRSThread = new RSThreadCalc[m_picNames.count()];
		m_nThreadFinish = 0;//Record how many thread has finished.
		for (int i = 0; i < m_picNames.count(); ++i)
		{
			connect( &m_pRSThread[i], SIGNAL( signalCalcStatus(bool, bool, bool, QString, QString) ),
				this,  SLOT( slot_CalcImport(bool, bool, bool, QString, QString) ) );
			connect( &m_pRSThread[i], SIGNAL( finished() ), this, SLOT( slot_ThreadFinished() ) );
			connect( this, SIGNAL( closeThread() ), &m_pRSThread[i], SLOT( stop() ) );//emit closeThread() in closeEvent

			strPicPath = m_picNames[i];
			dir.setPath(m_picNames.at(i));
			m_picNames[i] = dir.dirName();
			nIndex = m_picNames[i].find(".");
			QFile::copy(strPicPath, strPro + m_picNames[i]);
			wReadInfoEdit->append(QString::fromLocal8Bit("拷贝图片")  + m_picNames[i] + QString::fromLocal8Bit("成功"));
			wReadInfoEdit->append(QString::fromLocal8Bit("分析图片") +m_picNames[i] + QString::fromLocal8Bit("请等待..."));

			strTemp = m_picNames[i];
			m_sSumPath.append( strPro + strTemp.insert(nIndex, "_por") );
			strTemp = m_picNames[i];
			m_sSumPath.append( strPro + strTemp.insert(nIndex, "_rgb") );		
			strTemp = m_picNames[i];
			m_sSumPath.append( strPro + strTemp.insert(nIndex, "_dem") );			
			strTemp = m_picNames[i];
			m_sSumPath.append( strPro + strTemp.remove(nIndex, 10).insert(nIndex, ".txt") );

			m_pRSThread[i].setParameter(fpor, fdrm, frrf, fklp, fdmin, strPicPath, m_picNames[i], m_sSumPath);//Run another thread.
			m_sSumPath.clear();
			wPictureBtn->setEnabled(false);
			wTerminateBtn->setEnabled(true);
		}
	}
	else
	{
		wReadInfoEdit->append("No picture was found, update the database failed !");
    }
}

/**************************************************************************************************
* Purpose: 待所有子线程finished，进行清理工作.
* Parameter: void
* Return: void
* Throws: NULL
* Note: 
* Others:
**************************************************************************************************/
void RSPoreStructureAnalysis::slot_ThreadFinished()
{
	m_nThreadFinish++;
	if (m_picNames.count() == m_nThreadFinish)//All finished now.
	{
		if (m_pRSThread)
		{
			delete []m_pRSThread;
			m_pRSThread = NULL;
		}
		wPictureBtn->setEnabled(true);
		wTerminateBtn->setEnabled(false);
	}
}

/**************************************************************************************************
* Purpose: 每个线程完毕都会发射信号，连接到这个信号槽.
  会根据信号传出结果进行分析，读取txt文件数据求出均值存入数据库中.
* Parameter: bool | bFir, bSec, bThd | [IN] 算法计算成功与否（三个算法）
                    QString | strPicName | [IN]图片路径
					QString | strTxtName | [IN]生成txt路径
* Return: void
* Throws: NULL
* Note: 注意使用线程锁，此函数里数据并非全部线程安全的.
* Others:
**************************************************************************************************/
void RSPoreStructureAnalysis::slot_CalcImport(bool bFir, bool bSec, bool bThd, QString strPicName, QString strTxtName)
{
	QString strTemp;
	int nIndex = strPicName.find(".");
	if (bFir)
	{
		m_Mutex.lock();
		strTemp = strPicName;
		wReadInfoEdit->append( QString::fromLocal8Bit("生成图片") + strTemp.insert(nIndex, "_por") + QString::fromLocal8Bit("成功") );
		strTemp = strPicName;
		wReadInfoEdit->append( QString::fromLocal8Bit("生成图片") + strTemp.insert(nIndex, "_rgb") + QString::fromLocal8Bit("成功") );
		m_Mutex.unlock();
	}
	else
	{
		m_Mutex.lock();
		strTemp = strPicName;
		wReadInfoEdit->append( QString::fromLocal8Bit("生成图片") + strTemp.insert(nIndex, "_por") + QString::fromLocal8Bit("失败") );
		strTemp = strPicName;
		wReadInfoEdit->append( QString::fromLocal8Bit("生成图片") + strTemp.insert(nIndex, "_rgb") + QString::fromLocal8Bit("失败") );
		m_Mutex.unlock();
	}
	if (bSec)
	{
		m_Mutex.lock();
		strTemp = strPicName;
		wReadInfoEdit->append( QString::fromLocal8Bit("生成图片") + strTemp.insert(nIndex, "_dem") + QString::fromLocal8Bit("成功") );
		m_Mutex.unlock();
	}
	else
	{
		m_Mutex.lock();
		strTemp = strPicName;
		wReadInfoEdit->append( QString::fromLocal8Bit("生成图片") + strTemp.insert(nIndex, "_dem") + QString::fromLocal8Bit("失败") );
		m_Mutex.unlock();
	}
	float fSum[5];
	if (bThd)
	{
		strTemp = strPicName;
		m_Mutex.lock();
		wReadInfoEdit->append( QString::fromLocal8Bit("生成图片") + strTemp.remove(nIndex, 10).insert(nIndex, ".txt") + QString::fromLocal8Bit("成功") );
		QFile data(strTxtName);
		if (data.open(QFile::ReadOnly | QIODevice::Text))
		{
			QTextStream out(&data);
			int nIndex = 0;
			float fTemp[5];
			for (int j = 0; j < 5; ++j)
			{
				fSum[j] = 0;
				fTemp[j] = 0;
			}
			QString strType;
			out>>strType;
			for (int j = 0; j < 1001; ++j)
			{
				out>>nIndex>>fTemp[0]>>fTemp[1]>>fTemp[2]>>fTemp[3]>>fTemp[4];
				for (int k = 0; k < 5; ++k)
				{
					fSum[k]+=fTemp[k];
				}
			}
			for (int j = 0; j < 5; ++j)
			{
				fSum[j]/=1001;
			}
		}
		data.close();

		int nRowNum = 0;
		nRowNum = m_pModel->rowCount();
		m_pModel->insertRow(nRowNum);
		wReadInfoEdit->append(QString::fromLocal8Bit("扫描图片") + strPicName +QString::fromLocal8Bit("已完成，数据库更新成功"));
		m_pModel->setData(m_pModel->index(nRowNum, 0), strPicName);
		if (!wWellEdit->text().isEmpty())
		{
			m_pModel->setData(m_pModel->index(nRowNum, 1), wWellEdit->text());
			wReadInfoEdit->append(QString::fromLocal8Bit("导入井") + wWellEdit->text() +QString::fromLocal8Bit("数据成功"));
		}
		else
		{
			wReadInfoEdit->append(QString::fromLocal8Bit("导入井") + wWellEdit->text() + QString::fromLocal8Bit("数据失败"));
		}
		m_pModel->setData(m_pModel->index(nRowNum, 1), wWellEdit->text());
		for (int j = 0; j < 5; ++j)
		{
			m_pModel->setData(m_pModel->index(nRowNum, j+13), fSum[j]);
		}
		/*Warning:
		     Only at Insert Delete Submit Save these four operations the model directly submit to database ,
		otherwise the model will submit when save(submit) button is clicked!
		This method means you can revert at any time before save(sunmit) is clicked ! */
		m_pModel->submitAll();
		wReadInfoEdit->append(QString::fromLocal8Bit("更新数据库成功"));
		m_Mutex.unlock();
	}
	else
	{
		m_Mutex.lock();
		strTemp = strPicName;
		wReadInfoEdit->append( QString::fromLocal8Bit("生成文件") + strTemp.remove(nIndex, 10).insert(nIndex, ".txt") + QString::fromLocal8Bit("失败") );
		m_Mutex.unlock();
	}
}

/**************************************************************************************************
* Purpose: 任意时刻终止线程，但会让线程善终.
* Parameter: void
* Return: void
* Throws: NULL
* Note: stop()函数中有等待线程结束的wait()函数(Is Necessary?)，所以每次循环均会完成一个线程.
* Others:
**************************************************************************************************/
void RSPoreStructureAnalysis::slot_TerminateClicked()
{
	for (int i=0; i<m_picNames.count(); ++i)
	{
		m_pRSThread[i].stop();
	}
}

void RSPoreStructureAnalysis::slot_EditAction()
{
	RSTableEditor editor(m_pModel);
	editor.setWindowFlags(Qt::Widget);
	editor.show();
	editor.exec();
}

void RSPoreStructureAnalysis::slot_PopAction()
{
	QDialog *pDialog = new QDialog;
	pDialog->setWindowFlags(Qt::Widget);//Can Maximize or Minimize.
	QTableView *pView = new QTableView;
	pView->setModel(m_pModel);
	QHBoxLayout *pMainLayout = new QHBoxLayout;
	pMainLayout->addWidget(pView);
	pDialog->setLayout(pMainLayout);
	pDialog->setWindowTitle(QString::fromLocal8Bit("编辑"));
	pDialog->resize(600,460);
	QItemSelectionModel *selections = wTableView->selectionModel();
	QModelIndexList selecteds = selections->selectedIndexes();
	//QModelIndexList selecteds = wTableView->selectedIndexes();//Cannot access protected member declared in class QTableView.
	int nRow = m_pModel->rowCount();
	int nCol = m_pModel->columnCount();
	bool bIsHide = true;
	for (int i = 0; i < nRow; ++i)
	{
		bIsHide = true;
		foreach(QModelIndex index, selecteds)
		{
			if (index.row() == i)
			{
				bIsHide = false;
				break;
			}
		}
		if (bIsHide)
		{
			pView->hideRow(i);
		}	
	}
	for (int i = 0; i < nCol; ++i)
	{
		bIsHide = true;
		foreach(QModelIndex index, selecteds)
		{
			if (index.column() == i)
			{
				bIsHide = false;
				break;
			}	
		}
		if (bIsHide)
		{
			pView->hideColumn(i);
		}		
	}
	pDialog->show();
	pDialog->move((QApplication::desktop()->width() -pDialog->width())/2, (QApplication::desktop()->height() -pDialog->height())/2);
	pDialog->exec();
	delete pDialog;
}

void RSPoreStructureAnalysis::slot_SortAction()
{
	m_pModel->setSort(m_nSortcol,Qt::AscendingOrder);
	m_pModel->select();
}

void RSPoreStructureAnalysis::slot_ShowMenu(const QPoint& pos)
{
	Q_UNUSED (pos);
	if(m_pMenu)
	{
		m_pMenu->exec(QCursor::pos());
	}
}

void RSPoreStructureAnalysis::slot_ShowHeaderMenu(const QPoint& pos)
{
	m_nSortcol = wTableView->horizontalHeader()->logicalIndexAt(pos);//Get which column is clicked!
	if(m_pHeaderMenu)
	{
		m_pHeaderMenu->exec(QCursor::pos());
	}
}

void RSPoreStructureAnalysis::slot_ImportDataClicked()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("File (*.txt)"));
	dialog.setViewMode(QFileDialog::List);
	QStringList strFile;
	if (dialog.exec())
	{
		strFile = dialog.selectedFiles();
	}
	if (strFile.isEmpty())
	{
		return;
	}
	QFile file(strFile.at(0));
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream in(&file);
	/*读取txt以追加的方式添加至数据库表格*/
	while (!in.atEnd()) 
	{
		QString strLine = in.readLine();
		QStringList strList = strLine.split(QRegExp("\\s+"));
		int nCol = strList.count();
		int nCurRow = m_pModel->rowCount();
		m_pModel->insertRow(nCurRow);
		for(int i = 0; i < nCol; ++i)
		{
			QModelIndex idx = m_pModel->index(nCurRow, i);
			m_pModel->setData(idx, QVariant(strList.at(i)));
			if (i == m_pModel->columnCount())
			{
				break;
			}
		}
	}
	m_pModel->submitAll();
}

void RSPoreStructureAnalysis::slot_SaveBtnClicked()
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

/**************************************************************************************************
* Purpose: 线性的拟合算法
* Parameter: void
* Return: void
* Throws: NULL
* Note: 
* Others:
**************************************************************************************************/
bool linerRegression(float *lpVs, float *lpVp, int nCount, float &a, float &b)
{
	int i;
	double lx,ly,lx2,lxy;
	double lVp,lVs;
	double la,lb;
	if(nCount<=0)
		return false;
	lx=0;
	ly=0;
	lx2=0;
	lxy=0;
	for(i=0;i<nCount;i++)
	{
		lVp=(double)lpVp[i];
		lVs=(double)lpVs[i];
		lx+=lVp;
		ly+=lVs;
		lxy+=lVs*lVp;
		lx2+=lVp*lVp;
	}
	la=(nCount*lxy-lx*ly)/(nCount*lx2-lx*lx);
	lb=(ly-la*lx)/nCount;

	a=(float)la;
	b=(float)lb;
	return true;
}

/**************************************************************************************************
* Purpose: 调用交汇图的底层，显示出任意选择的两(ExistUnique)列的点
				以及根据此二列点线性插值来的曲线。
* Parameter: void
* Return: void
* Throws: NULL
* Note: 
* Others:
**************************************************************************************************/
void RSPoreStructureAnalysis::slot_CrossPlotClicked()
{
	QItemSelectionModel *selections = wTableView->selectionModel();
	QModelIndexList selecteds = selections->selectedIndexes();
	if (selecteds.isEmpty())
	{
		QMessageBox::warning(this, "Waring", "Please selected two pColumns.");
		return;
	}
	int nColFir = 0;
	int nColSed = 0;
	nColFir = selecteds.at(0).column();
	foreach(QModelIndex index, selecteds)
	{
		if (index.column() != nColFir)
		{
			nColSed = index.column();
			break;
		}
	}
	foreach(QModelIndex index, selecteds)
	{
		if (index.column() != nColFir && index.column() != nColSed || nColSed == 0)
		{
			QMessageBox::warning(this, "Waring", "Please selected two columns.");
			return;
		}	
	}

	int nRowCount = m_pModel->rowCount();
	float *pfLogX = new float[nRowCount];
	float *pfLogY = new float[nRowCount];
	QVariant var = QVariant();
	QModelIndex idx = QModelIndex();
	for (int i = 0; i < nRowCount; ++i)
	{
		idx = m_pModel->index(i, nColFir);
		var = m_pModel->data(idx);
		pfLogX[i] = var.toFloat();
		idx = m_pModel->index(i, nColSed);
		var = m_pModel->data(idx);
		pfLogY[i] = var.toFloat();
	}
	
	float fxMin,fyMin, fxMax, fyMax;
	fxMax = fyMax = -999999.9f;
	fxMin = fyMin = 999999.9f;
	for (int i = 0; i < nRowCount; ++i)
	{
		if (pfLogX[i] < fxMin)
		{
			fxMin = pfLogX[i];
		}
		if (pfLogX[i] > fxMax)
		{
			fxMax = pfLogX[i];
		}
		if (pfLogY[i] < fyMin)
		{
			fyMin = pfLogY[i];
		}
		if (pfLogY[i] > fyMax)
		{
			fyMax = pfLogY[i];
		}
	}
	RSCrossPlotMainWin *pCPWin;
	RSCrossPlotDlg	*m_pCrossPlotDlg;
	m_pCrossPlotDlg = new RSCrossPlotDlg(this);
	m_pCrossPlotDlg->setWindowModality(Qt::NonModal);
	m_pCrossPlotDlg->setCrossPlotNum(4,2,2);
	m_pCrossPlotDlg->setModal(false);
	m_pCrossPlotDlg->resize(600, 460);
	m_pCrossPlotDlg->getCrossPlotMainWin(0)->hide();
	m_pCrossPlotDlg->getCrossPlotMainWin(2)->hide();
	m_pCrossPlotDlg->getCrossPlotMainWin(3)->hide();
	pCPWin = m_pCrossPlotDlg->getCrossPlotMainWin(1);
	pCPWin->setMinimumWidth(400);
	pCPWin->setMinimumHeight(300);

	RSCPAttrList *pAttrList;
	RSCPDataAttr *pDataAttr;
	pAttrList = new RSCPAttrList();
	pDataAttr = new RSCPDataAttr();
	pDataAttr->setData(pfLogX, pfLogY, nRowCount);
	QColor color = Qt::black;
	pDataAttr->setLineType(RSCP::NoCurve, color);
	pDataAttr->setPointType(RSCP::Ellipse, color, 8);
	pAttrList->append(pDataAttr);


	/*判断是否加入趋势线*/
	int ret = QMessageBox:: warning(this, tr( "Cross Plot"),
		tr("Do you want to display the selected points with fitline?"),
		QMessageBox::Ok | QMessageBox:: Cancel);
	if (ret == QMessageBox::Ok)
	{
		float fa = 0.0, fb = 0.0;//线性插值所计算出的的系数
		linerRegression(pfLogX, pfLogY, nRowCount, fa, fb);
		for (int i = 0; i < nRowCount; ++i)
		{
			pfLogY[i] = fa*pfLogX[i] + fb;
		}
		pDataAttr = new RSCPDataAttr();
		pDataAttr->setData(pfLogX, pfLogY, nRowCount);
		color = Qt::red;
		pDataAttr->setLineType(RSCP::Lines, color);
		pDataAttr->setPointType(RSCP::Ellipse, color, 6);
		pAttrList->append(pDataAttr);
		var = m_pModel->headerData(nColFir, Qt::Horizontal);
		QString sTypeX =var.toString();
		sTypeX = QString(sTypeX + "   Y = %1*X + %2").arg(fa).arg(fb);
		var = m_pModel->headerData(nColSed, Qt::Horizontal);
		QString sTypeY =var.toString();
		pCPWin->setScaleTitle(RSCP::LeftScale, sTypeY);
		pCPWin->setScaleTitle(RSCP::BottomScale, sTypeX);
	}
	else
	{
		var = m_pModel->headerData(nColFir, Qt::Horizontal);
		QString sTypeX =var.toString();
		var = m_pModel->headerData(nColSed, Qt::Horizontal);
		QString sTypeY =var.toString();
		pCPWin->setScaleTitle(RSCP::LeftScale, sTypeY);
		pCPWin->setScaleTitle(RSCP::BottomScale, sTypeX);
	}
	if (!m_pCrossPlotDlg->isShown()) m_pCrossPlotDlg->show();
	pCPWin->addData(pAttrList);
	pCPWin->showByRange(RSCP::LeftScale, 0.8*fyMin, 1.2*fyMax);
	pCPWin->showByRange(RSCP::BottomScale, 0.8*fxMin, 1.2*fxMax);
	pCPWin->canDrawPolygon(false);
	pCPWin->showLegend(false);
	pCPWin->updateUI();
	pAttrList->removeData();
	delete []pfLogX;
	delete []pfLogY;
	delete pAttrList;
}