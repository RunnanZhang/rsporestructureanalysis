/**************************************************************************************************
*FILE: rssqltablemodel.h
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
	//�����������麯������дModel������д�������ͣ���ʽ����ȡ���ݡ��������ݣ�
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	bool insertRow (int row, const QModelIndex &parent = QModelIndex());
	void initCheckVec();

private:
	//��¼�ڼ������CheckBox
	int m_nColNumWithCheckBox;
	//��¼��CheckBox�����е�ѡ�����.
	QVector<bool> checkVec;
};
#endif