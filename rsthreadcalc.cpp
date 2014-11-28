/**************************************************************************************************
*FILE: rsthreadcalc.cpp
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
#include "rsthreadcalc.h"

RSThreadCalc::RSThreadCalc(QObject *parent)
: QThread(parent)
{
	m_fpor = 0;
	m_fdrm = 0;
	m_frrf = 0;
	m_fklp = 0;
	m_fdmin = 0;
}

RSThreadCalc::~RSThreadCalc()
{

}

void RSThreadCalc::setParameter(float fpor, float fdrm, float frrf, float fklp, float fdmin,
								QString strPicPath, QString strPicName, QStringList sSum)
{
	m_bThread = true;
	m_fpor = fpor;
	m_fdrm = fdrm;
	m_frrf = frrf;
	m_fklp = fklp;
	m_fdmin = fdmin;
	m_strPicPath = strPicPath;
	m_strPicName = strPicName;
	m_sSum = sSum;
	if (!isRunning())
	{
		start();
	}
}

void RSThreadCalc::stop()
{
	m_bThread = false;
	wait();
}

void RSThreadCalc::run()
{
	bool bFir, bSec, bThd;
	bFir = rsScanningElectronMicroscopePictrue(m_strPicPath.ascii(), m_fpor, m_fdrm, m_sSum[0].ascii(), m_sSum[1].ascii(), 1, m_bThread);
	if (!m_bThread)
	{
		bFir = false;
		bSec = false;
		bThd = false;
		emit signalCalcStatus(bFir, bSec, bThd, m_strPicName, m_sSum[3]);
		return;
	}
	bSec = rsGetElectronMicroscopePictrue(m_strPicPath.ascii(), m_frrf, m_fdrm, m_fdmin, m_fpor, m_fklp, m_sSum[2].ascii(), 1, m_bThread);
	if (!m_bThread)
	{
		bSec = false;
		bThd = false;
		emit signalCalcStatus(bFir, bSec, bThd, m_strPicName, m_sSum[3]);
		return;
	}
	bThd = rsGetElectronMicroscopePara(m_strPicPath.ascii(), m_fdrm, m_sSum[3].ascii(), m_bThread);
	if (!m_bThread)
	{
		bThd = false;
		emit signalCalcStatus(bFir, bSec, bThd, m_strPicName, m_sSum[3]);
		return;
	}
	emit signalCalcStatus(bFir, bSec, bThd, m_strPicName, m_sSum[3]);
}