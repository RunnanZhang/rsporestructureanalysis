/**************************************************************************************************
*FILE: rsthreadcalc.h
*DESCRIPTION: �߳��࣬���ڽ���㷨���㡣
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
*Note: ������߳����գ�������ɾ���߳�����ѡ��Deletelater()���ɱ�֤�˳���Ϣѭ�����Զ�ɾ��.
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
	//�߳̽��������ź�֪ͨ���̣߳�������Ӧ�������Ƿ����ɹ��ȣ�.
	void signalCalcStatus(bool bFir, bool bSec, bool bThd, QString strPicName, QString strTxtName);

public slots:
	void stop();

protected:
	void run();

private:
	QMutex m_Mutex;
	//������ͼƬ����·��.
	QString m_strPicPath;
	//ͼƬ���֣�����׺��.
	QString m_strPicName;
	//���ͼƬ�ļ��ľ���·��.
	QStringList m_sSum;
	//�㷨���ñ���.
	float m_fpor, m_fdrm, m_frrf, m_fklp, m_fdmin;
	//�̱߳�ʶ����Ϊ�˴ﵽ�߳����յ�Ŀ��.
	bool m_bThread;
};
#endif