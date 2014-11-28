#ifndef RSANALYTICALMETHODOFROCKSAMPLE_H
#define RSANALYTICALMETHODOFROCKSAMPLE_H

bool rsScanningElectronMicroscopePictrue(const char* pcPicname,float fPor, float fDrm,const char*pcNewPicnamePor, const char*pcNewPicname, int nIp, bool &bThread);

bool rsGetElectronMicroscopePictrue(const char* pcPicname,float rrf, float fDrm,float dmin, float fPor, float klp, const char*pcNewPicname, int nIp, bool &bThread);

bool rsGetElectronMicroscopePara(const char* pcPicname, float fDrm, const char*ParaTxt, bool &bThread);

#endif