/**************************************************************************************************
*FILE: rsporestructureanalysis.h
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
#ifndef RSPORESTRUCTUREANALYSIS_H
#define RSPORESTRUCTUREANALYSIS_H

#include "ui_rsporestructureanalysis.h"
#include "rsthreadcalc.h"
#include <QSqlDatabase>

class RSUIProject;
class RSUIBasic;
class UIWell;
class RSSqlTableModel;
class RSComboBoxDelegate;

class RSPoreStructureAnalysis : public QMainWindow, public Ui_RSPoreStructureAnalysis
{
	Q_OBJECT

public:
	RSPoreStructureAnalysis(QWidget * parent = 0, Qt::WindowFlags f = 0);
	~RSPoreStructureAnalysis();
	void setLibName(const QString &strLibName);
	void setProject(RSUIProject * p);
	void closeEvent(QCloseEvent * e);
	void removeUIObject(RSUIBasic * pUI);

private slots:
	void slot_WellSelectClicked();
	void slot_PictureBtnClicked();
	void slot_TerminateClicked();
	void slot_ImportDataClicked();
	void slot_SaveBtnClicked();
	void slot_CrossPlotClicked();
	void slot_ShowMenu(const QPoint&);
	void slot_EditAction();
	void slot_ShowHeaderMenu(const QPoint&);
	void slot_PopAction();
	void slot_SortAction();
	void slot_CalcImport(bool, bool, bool, QString, QString);
	void slot_ThreadFinished();

private:
	void init();
	void destroy();
	//create the sqlite database in our project.
	bool createConnection();

signals:
	void closeLibrary(const QString &);
	void closeThread(void);

private:
	//Multithread
	RSThreadCalc* m_pRSThread;
	int m_nThreadFinish;
	QMutex m_Mutex;
	QString m_strLibraryName;
	RSUIProject *m_pUIProject;
	RSUIBasic *m_pSingleWellManager;
	UIWell *m_pWell;
	RSSqlTableModel *m_pModel;
	QSqlDatabase m_db;
	RSComboBoxDelegate *m_pDelegate;
	//图片名字（带后缀）.
	QStringList m_picNames;
	//输出图片文件的绝对路径.
	QStringList m_sSumPath;
	//Right-click menu for QTableView.
	QMenu *m_pMenu;
	//Belongs to m_pMenu.
	QAction *m_pEditAction;
	//Belongs to m_pMenu.The Action is as the same as m_pPopAction
	QAction *m_pPopupAction;
	//Right-click menu for QHeaderView.
	QMenu *m_pHeaderMenu;
	//Belongs to m_pHeaderMenu.
	QAction *m_pPopAction;
	//Belongs to m_pHeaderMenu.
	QAction *m_pSortAction;
	//Record which column is clicked!
	int m_nSortcol;
};
#endif
