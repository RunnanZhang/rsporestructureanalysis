/**************************************************************************************************
*FILE: rscomboboxdelegate.h
*DESCRIPTION: 代理类，用于在井名一栏弹出井列表功能。
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
*Note: The four virtual function executed sequence(if ascending order) : 
           double click->142->appear control->operate->click other item->32
**************************************************************************************************/
#ifndef RSCOMBOBOXDELEGATE_H
#define RSCOMBOBOXDELEGATE_H
#include <QItemDelegate>

class RSComboBoxDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	RSComboBoxDelegate(const QStringList &wellName, QObject *parent = 0);

	virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, 
		const QModelIndex &index) const;

	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;

private:
	QStringList slWellName;
};
#endif