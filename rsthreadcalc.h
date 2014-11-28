/**************************************************************************************************
*FILE: rsthreadcalc.h
*DESCRIPTION: 线程类，用于结合算法计算。
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
*Note: 务必让线程善终，主程序删除线程优先选择Deletelater()，可保证退出消息循环后自动删除.
**************************************************************************************************/
#ifndef RSTHREADCALC_H
#define RSTHREADCALC_H

#include "rsAnalyticalMethodOfRockSample.h"
#include <QMutex>
#include <QThread>
#include <QStringList>

class RSThreadCalc : public QThread
{
	Q_OBJECT

public:
	explicit RSThreadCalc(QObject *parent = 0);
	~RSThreadCalc();

	void setParameter(float fpor, float fdrm, float frrf, float fklp, float fdmin,
		QString strPicPath, QString strPicName, QStringList sSum);

signals:
	//线程结束发出信号通知主线程，传出相应参数（是否计算成功等）.
	void signalCalcStatus(bool bFir, bool bSec, bool bThd, QString strPicName, QString strTxtName);

public slots:
	void stop();

protected:
	void run();

private:
	QMutex m_Mutex;
	//待计算图片绝对路径.
	QString m_strPicPath;
	//图片名字（带后缀）.
	QString m_strPicName;
	//输出图片文件的绝对路径.
	QStringList m_sSum;
	//算法所用变量.
	float m_fpor, m_fdrm, m_frrf, m_fklp, m_fdmin;
	//线程标识符，为了达到线程善终的目的.
	bool m_bThread;
};
#endif