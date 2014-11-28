/**************************************************************************************************
*FILE: rssqltablemodel.h
*DESCRIPTION: Model类，用于管理数据，连接数据库，可与多个视口绑定(MVC)
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
#ifndef RSSQLTABLEMODEL_H
#define RSSQLTABLEMODEL_H

#include <QSqlTableModel>
#include<QVector>

class RSSqlTableModel : public QSqlTableModel
{
	Q_OBJECT
public:
	RSSqlTableModel(const int colNumberWithCheckBox, QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

public:
	//下面这三个虚函数是重写Model必须重写的三剑客（样式、读取数据、设置数据）
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	bool insertRow (int row, const QModelIndex &parent = QModelIndex());
	void initCheckVec();

private:
	//记录第几列添加CheckBox
	int m_nColNumWithCheckBox;
	//记录含CheckBox所在列的选中情况.
	QVector<bool> checkVec;
};
#endif