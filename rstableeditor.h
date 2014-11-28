/**************************************************************************************************
*FILE: rstableeditor.h
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
#ifndef RSTABLEEDITOR_H
#define RSTABLEEDITOR_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QPushButton;
class QSqlTableModel;
class RSSqlTableModel;
class QTableView;
QT_END_NAMESPACE

class RSTableEditor : public QDialog
{
    Q_OBJECT

public:
    RSTableEditor(RSSqlTableModel* model, QWidget *parent = 0);

private slots:
    void submit();
	void insertRow();
	void deleteRow();

private:
    QPushButton *m_pSubmitBtn;
    QPushButton *m_pRrevertBtn;
    QPushButton *m_pQuitBtn;
	QPushButton *m_pInsertBtn;
	QPushButton *m_pDeleteBtn;
    QDialogButtonBox *m_pButtonBox;
    RSSqlTableModel *m_pModel;
	QTableView *m_pView;
};
#endif
