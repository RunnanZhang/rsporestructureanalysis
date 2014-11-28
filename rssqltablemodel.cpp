/**************************************************************************************************
*FILE: rssqltablemodel.cpp
*DESCRIPTION: Model�࣬���ڹ������ݣ��������ݿ⣬�������ӿڰ�(MVC)
*MODULE NAME: ��
*
*CREATE DATE: 2013-10-08
*COPYRIGHT (C): ����ŵ��˹��ʯ�ͿƼ����޹�˾
*AUTHOR: �����
*
*ENVIRONMENT: VS2008 Qt4.6.3
*
*CURRENT VERSION: GeoScope 2.9, �����, 2013-10-08
*VERSION HISTORY:
*
*Note:
**************************************************************************************************/
#include "rssqltablemodel.h"

RSSqlTableModel::RSSqlTableModel(const int colNumberWithCheckBox, QObject *parent, QSqlDatabase db)
:m_nColNumWithCheckBox(colNumberWithCheckBox),QSqlTableModel(parent, db)
{

}

void RSSqlTableModel::initCheckVec()
{
	int rowNum = this->rowCount();
	for (int i=0; i<rowNum; ++i)
	{
		checkVec.append(false);
	}
}

bool RSSqlTableModel::insertRow (int row, const QModelIndex &parent)
{
	checkVec.insert(row, false);
	return QSqlTableModel::insertRow(row,parent);
}

Qt::ItemFlags RSSqlTableModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
	if (index.column() == m_nColNumWithCheckBox)
		flags |= Qt::ItemIsUserCheckable;
	else
		flags |= Qt::ItemIsEditable;
	return flags;
}

QVariant RSSqlTableModel::data(const QModelIndex &idx, int role) const
{
	QVariant value = QSqlTableModel::data(idx, role);
	if (role == Qt::CheckStateRole && idx.column() == m_nColNumWithCheckBox)
		return checkVec.at(idx.row()) ? Qt::Checked : Qt::Unchecked;
	else
		return value;
}

bool RSSqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if(!index.isValid())
		return false;
	QSqlTableModel::setData(index, value, role);
	if (role == Qt::CheckStateRole && index.column() == m_nColNumWithCheckBox)
	{
		checkVec[index.row()] = value.toBool();
	}
	return true;
}