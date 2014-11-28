/**************************************************************************************************
*FILE: rscomboboxdelegate.cpp
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
		   注意仅当单机其他item时才出发setmodel函数，此时才与model交互.
**************************************************************************************************/
#include "rscomboboxdelegate.h"
#include <QComboBox>
#include <QApplication>

RSComboBoxDelegate::RSComboBoxDelegate(const QStringList &wellName, QObject *parent) : QItemDelegate(parent)
{
	slWellName = wellName;
}

QWidget *RSComboBoxDelegate::createEditor(QWidget *parent,
									   const QStyleOptionViewItem &/* option */,
									   const QModelIndex &/* index */) const
{
	QComboBox *pEditor = new QComboBox(parent);
	pEditor->addItems(slWellName);
	pEditor->installEventFilter(const_cast<RSComboBoxDelegate*>(this));
	return pEditor;
}

void RSComboBoxDelegate::setEditorData(QWidget *editor,
									const QModelIndex &index) const
{
	QString strValue = index.model()->data(index, Qt::EditRole).toString();
	QComboBox *pComboBox = static_cast<QComboBox*>(editor);
	int nIndex = pComboBox->findText(strValue);
	pComboBox->setCurrentIndex(nIndex);
}

void RSComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
								   const QModelIndex &index) const
{
	QComboBox *pComboBox = static_cast<QComboBox*>(editor);
	QString strValue = pComboBox->currentText();
	model->setData(index, strValue, Qt::EditRole);
}

void RSComboBoxDelegate::updateEditorGeometry(QWidget *editor,
										   const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
	//could see where is (0, 0) and know setGeometry()'s feature
	//editor->setGeometry(QRect(0, 0, 100, 100));
}