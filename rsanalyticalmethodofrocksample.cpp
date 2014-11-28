#include "rsanalyticalmethodofrocksample.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <complex>
#include <qapplication.h>

const float pi = (float)3.14159; //给出pi的值

/*****************************************************************
* Purpose: 图像扫描并进行简单孔隙分析形成新的孔隙分析后的图片算法.
* Parameter: pcPicname | char * | 待扫描的岩样图片名.
*            fPor | float | 输入的孔隙度
*            fDrm | float | 岩样图片中除去备注说明处的宽度比列，实际扫描图片为实际宽度的fDrm倍.
*			 pcNewPicnamePor | char * | 图像孔隙分析后生成的新图片，信息为孔隙结构生成的新图片,即为：'X-POR.BMP'.
*			 pcNewPicname | char * | 图像孔隙分析后生成的新图片，新图片信息为孔隙和原始岩性图片以何种方式显示.
*            nIp | int | 生成的图片是何种色素方式的显示标识；分别如下,其中X代表图片名称
                        nIp=1,'X-RGB.BMP';nIp=2,'X-RG.BMP';nIp=3,'-RB.BMP';
*						nIp=4,'X-GB.BMP'; nIp=5,'X-R.BMP';  nIp=6,'X-G.BMP'; nIp=7,'X-B.BMP';
*            bThread | bool | 是否有中间过程点击取消返回标识
* Return: returnType || the return value’s meaning.
* Throws: [List exceptions here] || [Why each exception is thrown]
* Note: make some note.
* Others: add some other notes.
*****************************************************************/
bool rsScanningElectronMicroscopePictrue(const char *pcPicname, float fPor, float fDrm, const char *pcNewPicnamePor, const char *pcNewPicname, int nIp, bool &bThread)
{
	FILE *fp0 = fopen(pcPicname, "r");   //读一个薄片图像

	if(!fp0)
	{
		printf("read picture failed, in function rsInitInterfacePictruePara");
		return false;
	}

	unsigned char ucA1[14];
	unsigned char ucA2[40];
	int ia1[14];
	int ia2[40];
	int jf[5];
	int jh[11];
	int mm = 0, nn = 0, m = 0, n = 0, mn = 0;
	int m0 = 0, mn0 = 0;
	int mp = 0;
	int k0 = 0, kk = 0;
	int m1 = 0, m2 = 0, m3 = 0, m4 = 0;
	int lv = 0, n1 = 0, n2 = 0, nn1 = 0, ll = 0, nn2 = 0, mn1 = 0;
	float rn2 = 0, rn1 = 0;
	float rn, rr;

	for(int i = 4; i < 12; i++)            //扫描图片，将图片对应色素字符串转成相应的ASCII码值
	{
		fseek(fp0, 14 + i, 0);
		fread(ucA2 + i, sizeof(unsigned char), 1, fp0);
		ia2[i] = ucA2[i];
	}

	nn = ((ia2[7] * 256 + ia2[6]) * 256 + ia2[5]) * 256 + ia2[4];    //计算出要分析图片中孔隙图的长宽，开辟数组用
	mm = ((ia2[11] * 256 + ia2[10]) * 256 + ia2[9]) * 256 + ia2[8];
	if (nn < 0 || nn > 1.0e7 || mm < 0 || mm >  1.0e7)
	{
		return false;
	}

	if(mm > m)
	{
		m = mm;
	}

	if(nn > n)
	{
		n = nn;
	}

	mn = m * n;
	if (mn < 0 || mn > 1.0e7)
	{
		return false;
	}
	//printf("m=%d,n=%d\n",m,n);
	unsigned char **r = new unsigned char *[m];
	unsigned char **g = new unsigned char *[m];
	unsigned char **b = new unsigned char *[m];

	for(int i = 0; i < m; i++)
	{
		r[i] = new unsigned char[n];
		g[i] = new unsigned char[n];
		b[i] = new unsigned char[n];
	}

	int **v = new int *[m];

	for(int i = 0; i < m; i++)
	{
		v[i] = new int[n];

		for(int j = 0; j < n; j++)
		{
			v[i][j] = 0;
		}
	}

	short int *ri = new short int [mn];
	short int *gj = new short int [mn];
	short int *bk = new short int [mn];
	//int *lrgb = new int[mn];
	int *rgb  = new int[mn];
	unsigned char *pTmp1 = new unsigned char;
	unsigned char *pTmp2 = new unsigned char;
	FILE *fp1 = fopen(pcNewPicname, "w");   //将扫描分析后的薄片形成新的分析后图片

	if(!fp1)
	{
		printf("write picture failed, in function rsInitInterfacePictruePara");

		for(int i = 0; i < m; i++)
		{
			delete []r[i];
			delete []g[i];
			delete []b[i];
			delete []v[i];
		}

		delete []r;
		delete []g;
		delete []b;
		delete []v;
		delete []ri;
		delete []gj;
		delete []bk;
		delete []rgb;
		delete pTmp1;
		delete pTmp2;
		return false;
	}

	for(int i = 0; i < 14; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		fseek(fp0, i, 0);
		fread(ucA1 + i, sizeof(unsigned char), 1, fp0);
		fwrite(ucA1 + i, sizeof(unsigned char), 1, fp1);
		ia1[i] = ucA1[i];
	}

	for(int i = 14; i < 54; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		fseek(fp0, i, 0);
		fread(ucA2 + i - 14, sizeof(unsigned char), 1, fp0);
		fwrite(ucA2 + i - 14, sizeof(unsigned char), 1, fp1);
		ia2[i - 14] = ucA2[i - 14];
	}
    //-------------->根据薄片图片中的色素字符串得到的ASCII码值计算相应分析孔隙结构的参数
	jf[0] = ia1[1] * 256 + ia1[0];
	jf[1] = ((ia1[5] * 256 + ia1[4]) * 256 + ia1[3]) * 256 + ia1[2];
	jf[2] = ia1[7] * 256 + ia1[6];
	jf[3] = ia1[9] * 256 + ia1[8];
	jf[4] = ((ia1[13] * 256 + ia1[12]) * 256 + ia1[11]) * 256 + ia1[10];
	jh[0] = ((ia2[3] * 256 + ia2[2]) * 256 + ia2[1]) * 256 + ia2[0];
	jh[1] = ((ia2[7] * 256 + ia2[6]) * 256 + ia2[5]) * 256 + ia2[4];
	jh[2] = ((ia2[11] * 256 + ia2[10]) * 256 + ia2[9]) * 256 + ia2[8];
	jh[3] = ia2[13] * 256 + ia2[12];
	jh[4] = ia2[15] * 256 + ia2[14];
	jh[5] = ((ia2[19] * 256 + ia2[18]) * 256 + ia2[17]) * 256 + ia2[16];
	jh[6] = ((ia2[23] * 256 + ia2[22]) * 256 + ia2[21]) * 256 + ia2[20];
	jh[7] = ((ia2[27] * 256 + ia2[26]) * 256 + ia2[25]) * 256 + ia2[24];
	jh[8] = ((ia2[31] * 256 + ia2[30]) * 256 + ia2[29]) * 256 + ia2[28];
	jh[9] = ((ia2[35] * 256 + ia2[34]) * 256 + ia2[33]) * 256 + ia2[32];
	jh[10] = ((ia2[39] * 256 + ia2[38]) * 256 + ia2[37]) * 256 + ia2[36];
	m = jh[2];
	n = jh[1];
	m0 = m * fDrm;
	mn0 = m0 * n;
	mn = m * n;
	mp = (mn - mn0) * fPor;
	k0 = 0, kk = 0;
    //<-------------------------------------------------------------------
	//------------------->读图片中需要计算孔隙结构分析的相应参数，并进行相应的计算
	for(int i = 0; i < m; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 0; j < n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			k0 = i * n + j;
			kk = k0 * 3 + 53;
			fseek(fp0, kk + 1, 0);
			fread(&b[i][j], sizeof(unsigned char), 1, fp0);
			bk[k0] =  b[i][j];
			fseek(fp0, kk + 2, 0);
			fread(&g[i][j], sizeof(unsigned char), 1, fp0);
			gj[k0] = g[i][j];
			fseek(fp0, kk + 3, 0);
			fread(&r[i][j], sizeof(unsigned char), 1, fp0);
			ri[k0] = r[i][j];
		}
	}

	//m1=0,m2=0,m3=0,m4=0;
	m1 = m0 + 1;
	lv = 0;

	for(int i = m1 - 1; i < m; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 0; j < n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			v[i - m0][j] = 0;
		}
	}

	n1 = 0, n2 = 0, nn1 = 0;
	ll = 0, nn2 = 0;

	if(nIp == 1)
	{
		for(k0 = mn0; k0 < mn; k0++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			rgb[k0] = ri[k0] * gj[k0] + ri[k0] * bk[k0] + gj[k0] * bk[k0];
		}

		//printf("ri[mn0]=%d,gj[mn0]=%d,bk[mn0]=%d\n",ri[mn0],gj[mn0],bk[mn0]);
		//printf("k0=%d,rgb[mn0]=%d,rgb[mn-1]=%d\n",k0,rgb[mn0],rgb[mn-1]);
		n1 = 1;
		n2 = 256 * 256 * 3;
		nn1 = 0;
	}

	if(nIp == 2)
	{
		for(k0 = 0; k0 < mn; k0++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			rgb[k0] = ri[k0] * gj[k0];
		}

		n1 = 1;
		n2 = 256 * 256;
		nn1 = 0;
	}

	if(nIp == 3)
	{
		for(k0 = 0; k0 < mn; k0++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			rgb[k0] = ri[k0] * bk[k0];
		}

		n1 = 1;
		n2 = 256 * 256;
		nn1 = 0;
	}

	if(nIp == 4)
	{
		for(k0 = 0; k0 < mn; k0++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			rgb[k0] = gj[k0] * bk[k0];
		}

		n1 = 1;
		n2 = 256 * 256;
		nn1 = 0;
	}

	if(nIp == 5)
	{
		for(k0 = 0; k0 < mn; k0++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			rgb[k0] = ri[k0];
		}

		n1 = 1;
		n2 = 256;
		nn1 = 0;
	}

	if(nIp == 6)
	{
		for(k0 = mn0; k0 < mn; k0++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			rgb[k0] = gj[k0];
		}

		n1 = 1;
		n2 = 256;
		nn1 = 0;
	}

	if(nIp == 7)
	{
		for(k0 = 0; k0 < mn; k0++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			rgb[k0] = bk[k0];
		}

		n1 = 1;
		n2 = 256;
		nn1 = 0;
	}

	while(1)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		nn = (n1 + n2 + 1) / 2;
		ll = 0;

		for(int l = mn0; l < mn; l++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			if(rgb[l] <= nn)
			{
				ll = ll + 1;
			}
		}

		nn2 = ll;

		if(n2 - n1 <= 1)
		{
			break;
		}

		if(ll < mp)
		{
			n1 = nn;
		}
		else
		{
			n2 = nn;
		}

		nn1 = nn2;
	}

	//printf("nn=%d,ll=%d,n1=%d,n2=%d,nn1=%d,nn2=%d\n",nn,ll,n1,n2,nn1,nn2);
	rn2 = nn2 - nn1;
	rn1 = nn2 - mp;

	if(rn2 != 0)
	{
		rn = rn1 / rn2;
	}
	else
	{
		rn = 0;
	}

	mn1 = mn0 + 1;
	//printf("rn2=%f,rn1=%f,rn=%f,mn1=%d\n",rn2,rn1,rn,mn1);
	srand((unsigned)time(NULL));

	for(ll = mn1 - 1; ll < mn; ll++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		if(rgb[ll] == nn)
		{
			rr = rand() / 32767.0f;

			if(rr < rn)
			{
				rgb[ll] = nn + 1;
			}
		}
	}
    
	//------------------->将分析计算后的孔隙结构情况形成新的图片，并根据ip类型保存成相应的色素格式
	for(int i = 0; i < m; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 0; j < n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			ll = i * n + j;
			kk = ll * 3 + 53;

			if(rgb[ll] <= nn && ll > mn0)
			{
				if(nIp)
				{
					lv = lv + 1;
					v[i - m0][j] = 1;
				}

				*pTmp1 = 1;
				fseek(fp1, kk + 1, 0);
				fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);//fwrite(unsigned char(1),sizeof(unsigned char),1,fp1);
				fseek(fp1, kk + 2, 0);
				fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);//fwrite(unsigned char(1),sizeof(unsigned char),1,fp1);
				*pTmp2 = 255;
				fseek(fp1, kk + 3, 0);
				fwrite((void *)pTmp2, sizeof(unsigned char), 1, fp1);//fwrite(unsigned char(255),sizeof(unsigned char),1,fp1);
			}
			else
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				fseek(fp1, kk + 1, 0);
				fwrite(&b[i][j], sizeof(unsigned char), 1, fp1);
				fseek(fp1, kk + 2, 0);
				fwrite(&g[i][j], sizeof(unsigned char), 1, fp1);
				fseek(fp1, kk + 3, 0);
				fwrite(&r[i][j], sizeof(unsigned char), 1, fp1);
			}
		}
	}

	if(m0 >= 0)
	{
		for(int j = 0; j < n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			ll = m0 * n + j;
			kk = ll * 3 + 53;
			/*fseek(fp1,kk+1,SEEK_CUR);
			fwrite(char(1),sizeof(char),1,fp1);
			fseek(fp1,kk+2,SEEK_CUR);
			fwrite(char(1),sizeof(char),1,fp1);
			fseek(fp1,kk+3,SEEK_CUR);
			fwrite(char(1),sizeof(char),1,fp1);*/
			*pTmp1 = 1;
			fseek(fp1, kk + 1, 0);
			fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);
			fseek(fp1, kk + 2, 0);
			fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);
			fseek(fp1, kk + 3, 0);
			fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);
		}
	}

	fclose(fp1);
	//<-----------------------------------------------------------------

	//--------------->扫描分析计算出图片中的孔隙，并保存成孔隙类型的图片
	FILE *fp2 = fopen(pcNewPicnamePor, "w");

	if(!fp2)
	{
		printf("write picture failed, in function rsInitInterfacePictruePara");

		for(int i = 0; i < m; i++)
		{
			delete []r[i];
			delete []g[i];
			delete []b[i];
			delete []v[i];
		}

		delete []r;
		delete []g;
		delete []b;
		delete []v;
		delete []ri;
		delete []gj;
		delete []bk;
		delete []rgb;
		delete pTmp1;
		delete pTmp2;
		return false;
	}

	jf[1] = jf[1] - mn0 * 3;
	jh[6] = jh[6] - mn0 * 3;
	m1 = jf[1];
	m2 = m1 / 256;
	m3 = m2 / 256;
	m4 = m3 / 256;
	ia1[2] = m1 - m2 * 256;
	ia1[3] = m2 - m3 * 256;
	ia1[4] = m3 - m4 * 256;
	ia1[5] = m4;

	for(int l = 2; l < 6; l++)
	{
		ucA1[l] = unsigned char(ia1[l]);
	}

	m1 = jh[6];
	m2 = m1 / 256;
	m3 = m2 / 256;
	m4 = m3 / 256;
	ia2[20] = m1 - m2 * 256;
	ia2[21] = m2 - m3 * 256;
	ia2[22] = m3 - m4 * 256;
	ia2[23] = m4;

	for(int l = 20; l < 24; l++)
	{
		ucA2[l] = unsigned char(ia2[l]);
	}

	m1 = m - m0;
	m2 = m1 / 256;
	m3 = m2 / 256;
	m4 = m3 / 256;
	ia2[8] = m1 - m2 * 256;
	ia2[9] = m2 - m3 * 256;
	ia2[10] = m3 - m4 * 256;
	ia2[11] = m4;

	for(int l = 8; l < 12; l++)
	{
		ucA2[l] = unsigned char(ia2[l]);
	}

	for(int l = 0; l < 14; l++)
	{
		fseek(fp2, l, 0);
		fwrite(ucA1 + l, sizeof(unsigned char), 1, fp2);
	}

	for(int l = 0; l < 40; l++)
	{
		fseek(fp2, l + 14, 0);
		fwrite(ucA2 + l, sizeof(unsigned char), 1, fp2);
	}

	for(int i = 0; i < m1; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 0; j < n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			ll = i * n + j;
			kk = (ll + 1) * 3 + 53;

			if(v[i][j] != 0)
			{
				*pTmp1 = 1;
				fseek(fp2, kk + 1, 0);
				fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp2);//fwrite(char(1),sizeof(char),1,fp2);
				fseek(fp2, kk + 2, 0);
				fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp2);//fwrite(char(1),sizeof(char),1,fp2);
				*pTmp2 = 255;
				fseek(fp2, kk + 3, 0);
				fwrite((void *)pTmp2, sizeof(unsigned char), 1, fp2);//fwrite(char(255),sizeof(char),1,fp2);
			}
			else
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				*pTmp2 = 255;
				fseek(fp2, kk + 1, 0);
				fwrite((void *)pTmp2, sizeof(unsigned char), 1, fp2);//fwrite(char(255),sizeof(char),1,fp2);
				*pTmp1 = 1;
				fseek(fp2, kk + 2, 0);
				fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp2);//fwrite(char(1),sizeof(char),1,fp2);
				fseek(fp2, kk + 3, 0);
				fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp2);//fwrite(char(1),sizeof(char),1,fp2);
			}
		}
	}

	//float aa0 = (float)lv/(float)(m1*n);
	//printf("porosity=%f,mp=%d,lv=%d,m1*n=%d,real(lv)/(m1*n)=%f\n",fPor,mp,lv,m1*n,aa0);
	fclose(fp2);
	fclose(fp0);

	for(int i = 0; i < m; i++)
	{
		delete []r[i];
		delete []g[i];
		delete []b[i];
		delete []v[i];
	}

	delete []r;
	delete []g;
	delete []b;
	delete []v;
	delete []ri;
	delete []gj;
	delete []bk;
	delete []rgb;
	delete pTmp1;
	delete pTmp2;
	return true;
}
/*****************************************************************
* Purpose: 图像孔隙分析算法.
* Parameter: pcPicname | char * | 待扫描的岩样图片名.
*            rrf | float | 
*            fDrm | float | 岩样图片中除去备注说明处的宽度比列，实际扫描图片为实际宽度的fDrm倍.
*            dmin | float |
*            pro | float | 输入的孔隙度
*            klp | float |
*			 pcNewPicname | char * | 图像孔隙分析后生成的新图片，新图片信息为孔隙和原始岩性图片的叠合，即孔隙结构、发育方向和密度
*            nIp | int | 选定相应色素显示图片进行孔隙分析，nIp代表的何种色素方式的显示标识如下,其中X代表图片名称
                        nIp=1,'X-POR.BMP';nIp=2,'X-RGB.BMP';nIp=3,'X-RG.BMP';nIp=4,'-RB.BMP';
*						nIp=5,'X-GB.BMP'; nIp=6,'X-R.BMP';  nIp=7,'X-G.BMP'; nIp=8,'X-B.BMP';
*            bThread | bool | 是否有中间过程点击取消返回标识
* Return: returnType || the return value’s meaning.
* Throws: [List exceptions here] || [Why each exception is thrown]
* Note: make some note.
* Others: add some other notes.
*****************************************************************/
bool rsGetElectronMicroscopePictrue(const char *pcPicname, float rrf, float fDrm, float dmin, float fPor, float klp, const char *pcNewPicname, int nIp, bool &bThread)
{
	FILE *fp0 = fopen(pcPicname, "r"); //读一个薄片图像

	if(!fp0)
	{
		printf("read picture failed, in function rsInitInterfacePictruePara");
		return false;
	}

	FILE *fp1 = fopen(pcNewPicname, "w");  //图像进行孔隙分析后输出新的分析后图片

	if(!fp1)
	{
		printf("write picture failed, in function rsInitInterfacePictruePara");
		return false;
	}

	float ap[51];
	float eth[51];
	float zw[51];
	float zg[361];
	float d[361];
	unsigned char ucA1[15];
	unsigned char ucA2[41];
	int ia1[15];
	int ia2[41];
	int la[51];
	int jf[6];
	int jh[12];
	rrf = rrf * 1000000.0f;
	float pa2 = pi / 2.0f;
	float dp = pa2 / (900 + 1);
	float dp2 = dp / 2.0f;
	int m = 0, n = 0, lp = 1; //mkp=1;
	float amin = (dmin / 2) * (dmin / 2) * pi;
	int mm = 0, nn = 0, mn = 0, ma = 0, mb = 0, mp = 0, mp0 = 0, ma1, ma2;
	int jh23, jf2, iie, jj, mj;
	int m0 = 0, mn0 = 0;
	int k0 = 0, kk = 0;
	int m1 = 0; //m2,m3=0,m4=0;
	int lv = 0, n1 = 0, n2 = 0, nn1 = 0, ll = 0, nn2 = 0, mn1 = 0;
	float rn2 = 0, rn1 = 0;
	float rn, rr;

	for(int i = 4; i < 12; i++)    //扫描图片，将图片对应色素字符串转成相应的ASCII码值
	{
		int l = i + 1;
		fseek(fp0, 14 + i, 0);
		fread(ucA2 + l, sizeof(unsigned char), 1, fp0);
		ia2[l] = ucA2[l];
	}

	nn = ((ia2[8] * 256 + ia2[7]) * 256 + ia2[6]) * 256 + ia2[5];       //计算出要分析图片中孔隙图的长宽，开辟数组用
	mm = ((ia2[12] * 256 + ia2[11]) * 256 + ia2[10]) * 256 + ia2[9];

	if (nn < 0 || nn > 1.0e7 || mm < 0 || mm >  1.0e7)
	{
		return false;
	}

	if(mm > m)
	{
		m = mm;
	}

	if(nn > n)
	{
		n = nn;
	}

	m = m + 178;
	mn = m * n;
	if (mn < 0 || mn > 1.0e7)
	{
		return false;
	}
	float index = (float)(m * m + n * n);
	ma = sqrt(index) + 1;
	mb = 50 * ma;
	//qWarning("m=%d,n=%d,ma=%d,mb=%d\n",m,n,ma,mb);
	short int *ri = new short int [mn + 1];
	short int *gj = new short int [mn + 1];
	short int *bk = new short int [mn + 1];
	int *rgb = new int[mn + 1];
	int *hpn = new int[mn + 1];
	int *hpm = new int[mn + 1];
	int *hpo = new int[mn + 1];
	int *hpk = new int[mn + 1];

	for(int i = 0; i <= mn; i++)
	{
		rgb[i] = 0;
		hpn[i] = 0;
		hpm[i] = 0;
		hpo[i] = 0;
		hpk[i] = 0;
	}

	int **v = new int *[m + 1];

	for(int i = 0; i <= m; i++)
	{
		v[i] = new int[n + 1];

		for(int j = 0; j <= n; j++)
		{
			v[i][j] = 0;
		}
	}

	float **zp = new float*[3];
	float **zs = new float*[3];

	for(int i = 0; i <= 2; i++)
	{
		zp[i] = new float[mb + 1];
		zs[i] = new float[mb + 1];

		for(int j = 0; j <= mb; j++)
		{
			zp[i][j] = 0;
			zs[i][j] = 0;
		}
	}

	int maindex = ma + 1;
	float *zq = new float[3 * maindex * 51];

	for(int i = 0; i < 3 * maindex * 51; i++)
	{
		zq[i] = 0;
	}

	int m_new = m;
	unsigned char **r = new unsigned char *[m + 1];
	unsigned char **g = new unsigned char *[m + 1];
	unsigned char **b = new unsigned char *[m + 1];
	unsigned char **e = new unsigned char *[m + 1];

	for(int i = 0; i <= m; i++)
	{
		r[i] = new unsigned char[n + 1];
		g[i] = new unsigned char[n + 1];
		b[i] = new unsigned char[n + 1];
		e[i] = new unsigned char[4];
	}

	unsigned char *pTmp1 = new unsigned char;
	unsigned char *pTmp2 = new unsigned char;
	int lp0 = 1;

	for(int i = 0; i <= 360; i++)
	{
		zg[i] = 0;
	}

	for(int i = 0; i < 14; i++)
	{
		int l = i + 1;
		fseek(fp0, i, 0);
		fread(ucA1 + l, sizeof(unsigned char), 1, fp0);
		ia1[l] = ucA1[l];
		//printf("i=%d,ia1[i]=%d\n",i,ia1[i]);
	}

	for(int i = 14; i < 54; i++)
	{
		int l = i + 1;
		fseek(fp0, i, 0);
		fread(ucA2 + l - 14, sizeof(unsigned char), 1, fp0);
		ia2[l - 14] = ucA2[l - 14];
		//printf("i=%d,ia2[i]=%d\n",i,ia2[i-14]);
	}
    
	//-------------->根据薄片图片中的色素字符串得到的ASCII码值计算相应分析孔隙结构的参数
	jf[1] = ia1[2] * 256 + ia1[1];
	jf[2] = ((ia1[6] * 256 + ia1[5]) * 256 + ia1[4]) * 256 + ia1[3];
	jf[3] = ia1[8] * 256 + ia1[7];
	jf[4] = ia1[10] * 256 + ia1[9];
	jf[5] = ((ia1[14] * 256 + ia1[13]) * 256 + ia1[12]) * 256 + ia1[11];
	/*for (int i=1;i<=5;i++)
	{
		printf("i=%d,jf[i]=%d\n",i,jf[i]);
	}*/
	jf[3] = (int)(fDrm * 100 + 0.00001);
	fDrm = (float)(jf[3]) / 100.0f;
	//printf("jf[3]=%d,fDrm=%f\n",jf[3],fDrm);
	ia1[8] = jf[3] / 256;
	ia1[7] = jf[3] - ia1[8] * 256;
	ucA1[7] = char(ia1[7]);
	ucA1[8] = char(ia1[8]);
	jh[1] = ((ia2[4] * 256 + ia2[3]) * 256 + ia2[2]) * 256 + ia2[1];
	jh[2] = ((ia2[8] * 256 + ia2[7]) * 256 + ia2[6]) * 256 + ia2[5];
	jh[3] = ((ia2[12] * 256 + ia2[11]) * 256 + ia2[10]) * 256 + ia2[9];
	jh[4] = ia2[14] * 256 + ia2[13];
	jh[5] = ia2[16] * 256 + ia2[15];
	jh[6] = ((ia2[20] * 256 + ia2[19]) * 256 + ia2[18]) * 256 + ia2[17];
	jh[7] = ((ia2[24] * 256 + ia2[23]) * 256 + ia2[22]) * 256 + ia2[21];
	jh[8] = ((ia2[28] * 256 + ia2[27]) * 256 + ia2[26]) * 256 + ia2[25];
	jh[9] = ((ia2[32] * 256 + ia2[31]) * 256 + ia2[30]) * 256 + ia2[29];
	jh[10] = ((ia2[36] * 256 + ia2[35]) * 256 + ia2[34]) * 256 + ia2[33];
	jh[11] = ((ia2[40] * 256 + ia2[39]) * 256 + ia2[38]) * 256 + ia2[37];
	jh23 = jh[2] * jh[3] * 3 + 54;
	jf2 = jf[2];
	iie = (jf2 - jh23) / jh[3];
	m = jh[3];
	n = jh[2];
	//printf("jf2=%d,iie=%d,m=%d,n=%d\n",jf2,iie,m,n);
	jj = 169;
	mj = m + jj;
	jf[2] = jf2 + jj * n * 3 + mj * iie;
	jh[3] = mj;
	jh[7] = mj * n * 3;
	ia1[6] = jf[2] / (256 * 256 * 256);
	ia1[5] = (jf[2] / (256 * 256) - ia1[6] * 256);
	ia1[4] = jf[2] / 256 - ia1[6] * 256 * 256 - ia1[5] * 256;
	ia1[3] = jf[2] - ia1[6] * 256 * 256 * 256 - ia1[5] * 256 * 256 - ia1[4] * 256;
	ia2[24] = jh[7] / (256 * 256 * 256);
	ia2[23] = (jh[7] / (256 * 256) - ia2[24] * 256);
	ia2[22] = jh[7] / 256 - ia2[24] * 256 * 256 - ia2[23] * 256;
	ia2[21] = jh[7] - ia2[24] * 256 * 256 * 256 - ia2[23] * 256 * 256 - ia2[22] * 256;
	ia2[12] = jh[3] / (256 * 256 * 256);
	ia2[11] = (jh[3] / (256 * 256) - ia2[12] * 256);
	ia2[10] = jh[3] / 256 - ia2[12] * 256 * 256 - ia2[11] * 256;
	ia2[9] = jh[3] - ia2[12] * 256 * 256 * 256 - ia2[11] * 256 * 256 - ia2[9] * 256;

    //----------------->将基础的图片信息首先写入
	for(int l = 0; l < 14; l++)
	{
		//printf("l+1=%d,ia1[l+1]=%d\n",l+1,ia1[l+1]);
		*pTmp1 = char(ia1[l + 1]);
		fseek(fp1, l, 0);
		fwrite((void *)pTmp1, sizeof(char), 1, fp1);
	}

	for(int l = 14; l < 54; l++)
	{
		//printf("l+1=%d,ia2[l+1]=%d\n",l+1,ia2[l-14+1]);
		*pTmp2 = ia2[l - 14 + 1];
		fseek(fp1, l, 0);
		fwrite((void *)pTmp2, sizeof(char), 1, fp1);
	}

	//------------------->读图片中需要计算孔隙结构分析的相应参数，并进行相应的计算
	if(jh[8] == 0)
	{
		jh[8] = 1;
	}

	if(jh[9] == 0)
	{
		jh[9] = 1;
	}

	ia2[29] = ia2[25];
	ia2[30] = ia2[26];
	ia2[31] = ia2[27];
	ia2[32] = ia2[28];
	float rxy = 1;
	float aa = (float)jh[8];
	float dy = (float)rrf / aa / klp;
	float d8s = 0.25 * sqrt(rxy * pi) * dy;
	int nain = amin / (rxy * dy * dy);
	m0 = m * fDrm;
	mn = m * n;
	mn0 = m0 * n;
	mp0 = (mn - mn0) * fPor;
	//printf("dy=%f,d8s=%f,nain=%d,m0=%d,mn=%d,mn0=%d,mp0=%d\n",dy,d8s,nain,m0,mn,mn0,mp0);
	ap[lp0] = (mn - mn0) * rxy * dy * dy;
	int nw0 = 0;

	for(int i = 1; i <= m; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			k0 = (i - 1) * n + j;
			kk = (k0 - 1) * 3 + 54 + (i - 1) * iie;
			fseek(fp0, kk, 0);
			fread(&b[i][j], sizeof(unsigned char), 1, fp0);
			bk[k0] =  b[i][j];
			fseek(fp0, kk + 1, 0);
			fread(&g[i][j], sizeof(unsigned char), 1, fp0);
			gj[k0] = g[i][j];
			fseek(fp0, kk + 2, 0);
			fread(&r[i][j], sizeof(unsigned char), 1, fp0);
			ri[k0] = r[i][j];
			//printf("i=%d,j=%d,ri[k0]=%d,gj[k0]=%d,bk[k0]=%d\n",i,j,ri[k0],gj[k0],bk[k0]);
		}

		for(int j = 1; j <= iie; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			fseek(fp0, kk + 2 + j, 0);
			fread(&e[i][j], sizeof(char), 1, fp0);
		}
	}

	//printf("i=%d,j=%d,ri[k0]=%d,gj[k0]=%d,bk[k0]=%d\n",1,1,ri[1],gj[1],bk[1]);
	//printf("i=%d,j=%d,ri[k0]=%d,gj[k0]=%d,bk[k0]=%d\n",m,n,ri[(m-1)*n+n],gj[(m-1)*n+n],bk[(m-1)*n+n]);
	ap[lp0] = (mn - mn0 - nw0) * rxy * dy * dy;
	int mp1 = 0;
	int mp2 = mn - mn0 - nw0;
	int npp = 0;
	int npor = 0, np0 = 0;
	int mmn = 0;
	int ij, i1, i2, j1, j2;
	int im0 = 0, jm = 0, ii = 0;

	while(1)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		mp = (mp1 + mp2) / 2;
		m1 = m0 + 1;
		lv = 0;

		for(int i = m1; i <= m; i++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			for(int j = 1; j <= n; j++)
			{
				v[i - m0][j] = 0;
			}
		}

		if(nIp == 1)
		{
			for(int k0 = mn0 + 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = ri[k0] * gj[k0] + ri[k0] * bk[k0] + gj[k0] * bk[k0];
			}

			//printf("ri[mn0]=%d,gj[mn0]=%d,bk[mn0]=%d\n",ri[mn0],gj[mn0],bk[mn0]);
			//printf("k0=%d,rgb[mn0]=%d,rgb[mn-1]=%d\n",k0,rgb[mn0],rgb[mn-1]);
			n1 = 1;
			n2 = 256 * 256 * 3;
			nn1 = 0;
		}

		if(nIp == 2)
		{
			for(int k0 = 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = ri[k0] * gj[k0];
			}

			n1 = 1;
			n2 = 256 * 256;
			nn1 = 0;
		}

		if(nIp == 3)
		{
			for(int k0 = 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = ri[k0] * bk[k0];
			}

			n1 = 1;
			n2 = 256 * 256;
			nn1 = 0;
		}

		if(nIp == 4)
		{
			for(int k0 = 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = gj[k0] * bk[k0];
			}

			n1 = 1;
			n2 = 256 * 256;
			nn1 = 0;
		}

		if(nIp == 5)
		{
			for(int k0 = 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = ri[k0];
			}

			n1 = 1;
			n2 = 256;
			nn1 = 0;
		}

		if(nIp == 6)
		{
			for(int k0 = mn0 + 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = gj[k0];
			}

			n1 = 1;
			n2 = 256;
			nn1 = 0;
		}

		if(nIp == 7)
		{
			for(int k0 = 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = bk[k0];
			}

			n1 = 1;
			n2 = 256;
			nn1 = 0;
		}

		while(1)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			nn = (n1 + n2 + 1) / 2;
			ll = 0;

			for(int l = mn0 + 1; l <= mn; l++)
			{
				if(rgb[l] <= nn)
				{
					ll = ll + 1;
				}
			}

			nn2 = ll;

			if(n2 - n1 <= 1)
			{
				break;
			}

			if(ll < mp)
			{
				n1 = nn;
			}
			else
			{
				n2 = nn;
			}

			nn1 = nn2;
		}

		//printf("nn=%d,ll=%d,n1=%d,n2=%d,nn1=%d,nn2=%d\n",nn,ll,n1,n2,nn1,nn2);
		rn2 = nn2 - nn1;
		rn1 = nn2 - mp;

		if(rn2 != 0)
		{
			rn = rn1 / rn2;
		}
		else
		{
			rn = 0;
		}

		mn1 = mn0 + 1;
		//printf("rn2=%f,rn1=%f,rn=%f,mn1=%d\n",rn2,rn1,rn,mn1);
		srand((unsigned)time(NULL));

		for(ll = mn1; ll <= mn; ll++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			if(rgb[ll] == nn)
			{
				rr = rand() / 32767.0f;

				if(rr < rn)
				{
					rgb[ll] = nn + 1;
				}
			}
		}

		for(int i = 1; i <= m; i++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			for(int j = 1; j <= n; j++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				ll = (i - 1) * n + j;
				kk = (ll - 1) * 3 + 54 + (i - 1) * iie;

				if(rgb[ll] <= nn && ll > mn0)
				{
					lv = lv + 1;
					v[i - m0][j] = lv;
				}
			}
		}

		mm = m - m0;
		mmn = mm * n;

		while(1)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			ij = 0;

			for(int i = 1; i <= mm; i++)
			{
				i1 = i - 1;
				i2 = i + 1;

				if(i1 < 1)
				{
					i1 = 1;
				}

				if(i2 > mm)
				{
					i2 = mm;
				}

				for(int j = 1; j <= n; j++)
				{
					if(!bThread)
					{
						break;//the thread is cancel.
					}

					if(v[i][j] != 0)
					{
						j1 = j - 1;
						j2 = j + 1;

						if(j1 < 1)
						{
							j1 = 1;
						}

						if(j2 > n)
						{
							j2 = n;
						}

						for(int i0 = i1; i0 <= i2; i0++)
						{
							if(!bThread)
							{
								break;//the thread is cancel.
							}

							for(int j0 = j1; j0 <= j2; j0++)
							{
								if(v[i0][j0] != 0 && v[i][j] > v[i0][j0])
								{
									ij = ij + 1;
									v[i][j] = v[i0][j0];
								}
							}
						}
					}
				}
			}

			if(ij == 0)
			{
				break;
			}
		}

		im0 = 0, jm = 0;

		for(int i = 1; i <= mmn; i++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			hpn[i] = 0;
		}

		for(int i = 1; i <= mm; i++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			for(int j = 1; j <= n; j++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				if(v[i][j] != 0)
				{
					ii = v[i][j];

					if(ii > im0)
					{
						im0 = ii;
					}

					hpn[ii] = hpn[ii] + 1;

					if(hpn[ii] > jm)
					{
						jm = hpn[ii];
					}
				}
			}
		}

		npor = 0, np0 = 0;

		for(int i = 1; i <= im0; i++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			np0 = np0 + hpn[i];

			if(hpn[i] >= nain)
			{
				npor = npor + hpn[i];
			}
		}

		if((mp2 - mp1) > 1 && abs(npor - mp0) > 1)
		{
			if(npor <= mp0)
			{
				mp1 = mp;
			}
			else
			{
				mp2 = mp;
			}

			continue;
		}

		if(npor < mp0 && npp == 0)
		{
			npp = 1;
			mp1 = mp2;
			continue;
		}
		else
		{
			break;
		}
	}

	//printf("npor=%d\n",npor);
	float rcut = 0.1 * (npor - mp0) / npor;
	np0 = 0;
	srand((unsigned)time(NULL));

	for(int i = 1; i <= mm; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			if(v[i][j] != 0)
			{
				rr = rand() / 32767.0f;

				if(rr < rcut)
				{
					v[i][j] = 0;
				}
			}

			if(v[i][j] != 0)
			{
				np0 = np0 + 1;
			}
		}
	}

	while(1)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		if(npor <= mp0)
		{
			break;
		}

		srand((unsigned)time(NULL));
		float rrx = rand() / 32767.0f;
		float rry = rand() / 32767.0f;
		int dy1 = (int)(1 + (m - 1) * rrx);
		int dy2 = (int)(1 + (n - 1) * rry);

		if(v[dy1][dy2] == 0)
		{
			continue;
		}

		i1 = dy1 - 1;
		i2 = dy1 + 1;
		j1 = dy2 - 1;
		j2 = dy2 + 1;

		if(i1 < 1)
		{
			i1 = 1;
		}

		if(i2 > mm)
		{
			i2 = mm;
		}

		if(j1 < 1)
		{
			j1 = 1;
		}

		if(j2 > n)
		{
			j2 = n;
		}

		if(v[i1][j1] == 0)
		{
			v[dy1][dy2] = 0;
			npor = npor - 1;
			np0 = np0 - 1;
			continue;
		}

		if(v[i1][dy2] == 0)
		{
			v[dy1][dy2] = 0;
			npor = npor - 1;
			np0 = np0 - 1;
			continue;
		}

		if(v[i1][j2] == 0)
		{
			v[dy1][dy2] = 0;
			npor = npor - 1;
			np0 = np0 - 1;
			continue;
		}

		if(v[dy1][j1] == 0)
		{
			v[dy1][dy2] = 0;
			npor = npor - 1;
			np0 = np0 - 1;
			continue;
		}

		if(v[dy1][j2] == 0)
		{
			v[dy1][dy2] = 0;
			npor = npor - 1;
			np0 = np0 - 1;
			continue;
		}

		if(v[i2][j1] == 0)
		{
			v[dy1][dy2] = 0;
			npor = npor - 1;
			np0 = np0 - 1;
			continue;
		}

		if(v[i2][dy2] == 0)
		{
			v[dy1][dy2] = 0;
			npor = npor - 1;
			np0 = np0 - 1;
			continue;
		}

		if(v[i2][j2] == 0)
		{
			v[dy1][dy2] = 0;
			npor = npor - 1;
			np0 = np0 - 1;
			continue;
		}
	}
    
	//------------------->将分析计算后的孔隙结构情况形成新的图片，并根据ip类型保存成相应的色素格式
	for(int i = 1; i <= m; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		int i0 = i - m0;

		for(int j = 1; j <= n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			k0 = (i - 1) * n + j;
			kk = (k0 - 1) * 3 + 54 + (i - 1) * iie;

			if(i > m0 && v[i0][j] != 0)
			{
				*pTmp1 = 1;
				fseek(fp1, kk, 0);
				fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);//fwrite(unsigned char(1),sizeof(unsigned char),1,fp1);
				fseek(fp1, kk + 1, 0);
				fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);//fwrite(unsigned char(1),sizeof(unsigned char),1,fp1);
				*pTmp2 = 255;
				fseek(fp1, kk + 2, 0);
				fwrite((void *)pTmp2, sizeof(unsigned char), 1, fp1);//fwrite(unsigned char(255),sizeof(unsigned char),1,fp1);
			}
			else
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				if(i == m0)
				{
					*pTmp1 = 1;
					fseek(fp1, kk, 0);
					fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);
					*pTmp2 = 255;
					fseek(fp1, kk + 1, 0);
					fwrite((void *)pTmp2, sizeof(unsigned char), 1, fp1);
					fseek(fp1, kk + 2, 0);
					fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);
				}
				else
				{
					fseek(fp1, kk, 0);
					fwrite(&b[i][j], sizeof(unsigned char), 1, fp1);
					fseek(fp1, kk + 1, 0);
					fwrite(&g[i][j], sizeof(unsigned char), 1, fp1);
					fseek(fp1, kk + 2, 0);
					fwrite(&r[i][j], sizeof(unsigned char), 1, fp1);
				}
			}
		}

		for(int j = 1; j <= iie; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			fseek(fp1, kk + 2 + j, 0);
			fwrite(&e[i][j], sizeof(unsigned char), 1, fp1);
		}
	}

	while(1)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		ij = 0;

		for(int i = 1; i <= mm; i++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			i1 = i - 1;
			i2 = i + 1;

			if(i1 < 1)
			{
				i1 = 1;
			}

			if(i2 > mm)
			{
				i2 = mm;
			}

			for(int j = 1; j <= n; j++)
			{
				if(v[i][j] != 0)
				{
					j1 = j - 1;
					j2 = j + 1;

					if(j1 < 1)
					{
						j1 = 1;
					}

					if(j2 > n)
					{
						j2 = n;
					}

					for(int i0 = i1; i0 <= i2; i0++)
					{
						for(int j0 = j1; j0 <= j2; j0++)
						{
							if(v[i0][j0] != 0 && v[i][j] > v[i0][j0])
							{
								ij = ij + 1;
								v[i][j] = v[i0][j0];
							}
						}
					}
				}
			}
		}

		if(ij == 0)
		{
			break;
		}
	}

	m = mm;
	mn = mmn;
	im0 = 0;
	jm = 0;

	for(int i = 1; i <= mmn; i++)
	{
		hpn[i] = 0;
	}

	for(int i = 1; i <= mm; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(v[i][j] != 0)
			{
				ii = v[i][j];

				if(ii > im0)
				{
					im0 = ii;
				}

				hpn[ii] = hpn[ii] + 1;

				if(hpn[ii] > jm)
				{
					jm = hpn[ii];
				}
			}
		}
	}

	int im = 0;

	for(ii = 1; ii <= im0; ii++)
	{
		if(hpn[ii] != 0)
		{
			im = im + 1;
			hpo[ii] = im;
			hpn[im] = hpn[ii];
		}
	}

	hpm[1] = 0;

	for(ii = 2; ii <= im; ii++)
	{
		hpm[ii] = hpn[ii - 1] + hpm[ii - 1];
	}

	for(ii = 1; ii <= im; ii++)
	{
		hpn[ii] = 0;
	}

	for(int i = 1; i <= mm; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(v[i][j] != 0)
			{
				ii = hpo[v[i][j]];
				v[i][j] = ii;
				hpn[ii] = hpn[ii] + 1;
				ll = hpm[ii] + hpn[ii];
				hpk[ll] = i * n + j + 1;
			}
		}
	}

	int iam = 0, jam = 0;

	for(ii = 1; ii <= mm; ii++)
	{
		if(hpn[ii] > jam)
		{
			jam = hpn[ii];
			iam = ii;
		}
	}

	ma1 = 0, ma2 = 0;
	eth[lp0] = 0;
	float aas, ra2, domg1, domg2;

	if(lp0 == 1)
	{
		aas = 0;
		ra2 = 0;
		domg1 = 0;
		domg2 = 0;
	}

	float aas0 = 0, ra20 = 0, ss = 0, elth = 0;
	float xi = 0, yi = 0, xyi = 0, xxi = 0;
	float xx = 0, yy = 0;
	float rrm, arm, grm;
	int idm;

	for(ii = 1; ii <= im; ii++)   //计算单个孔隙相应的孔隙方向角，发育长度
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		ss = hpn[ii] * rxy * dy * dy;
		jj = hpm[ii];
		ll = hpn[ii];
		elth = 0;

		if(ll < nain)
		{
			np0 = np0 - ll;
			continue;
		}

		xi = 0, yi = 0, xyi = 0, xxi = 0;

		for(int l = 1; l <= ll; l++)
		{
			kk = jj + l;
			int i = (hpk[kk] - 1) / n + 1;
			int j = hpk[kk] - (i - 1) * n;
			xi = xi + j;
			yi = yi + i * rxy;
			xyi = xyi + i * j * rxy;
			xxi = xxi + j * j;

			if(i == 1 || i == mm)
			{
				if(j == 1 || j == n)
				{
					elth = elth + 5 * d8s;
				}
				else
				{
					elth = elth + 3 * d8s;
				}
			}
			else
			{
				if(j == 1 || j == n)
				{
					elth = elth + 3 * d8s;
				}
			}

			i1 = i - 1;
			i2 = i + 1;
			j1 = j - 1;
			j2 = j + 1;

			if(i1 < 1)
			{
				i1 = 1;
			}

			if(j1 < 1)
			{
				j1 = 1;
			}

			if(i2 > mm)
			{
				i2 = mm;
			}

			if(j2 > n)
			{
				j2 = n;
			}

			for(int i0 = i1; i0 <= i2; i0++)
			{
				for(int j0 = j1; j0 <= j2; j0++)
				{
					if((i != i0 || j != j0) && v[i0][j0] == 0)
					{
						elth = elth + d8s;
					}
				}
			}
		}

		xx = xi * xi - ll * xxi;
		yy = xi * yi - ll * xyi;
		float gg = 0;
		srand((unsigned)time(NULL));

		if(xx == 0 && yy == 0)
		{
			rr = rand() / 32767.0f;
			gg = (rr - 0.5) * pi;
		}
		else
		{
			gg = atan2(yy, xx);
		}

		if(gg < 0)
		{
			gg = gg + pi;
		}

		int igg1 = (int)(gg * 90 / pa2 + 1.5);
		int igg2 = igg1 + 180;

		if(igg2 > 360)
		{
			igg2 = 1;
		}

		zg[igg1] = zg[igg1] + ll;
		zg[igg2] = zg[igg2] + ll;
		float doms = 0, xl = 0, yl = 0, sl = 0;
		float aa = 0, bb = 0;

		for(int l1 = 1; l1 <= ll; l1++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			kk = jj + l1;
			i1 = (hpk[kk] - 1) / n + 1;
			j1 = hpk[kk] - (i1 - 1) * n;

			for(int l2 = l1; l2 <= ll; l2++)
			{
				kk = jj + l2;
				i2 = (hpk[kk] - 1) / n + 1;
				j2 = hpk[kk] - (i2 - 1) * n;
				xl = (abs(i2 - i1) + 1) * rxy;
				yl = abs(j2 - j1) + 1;
				sl = xl * xl + yl * yl;

				if(sl > doms)
				{
					doms = sl;
				}
			}
		}

		doms = sqrt(doms);
		aa = doms * dy / 2;
		bb = ss / (pi * aa);

		if(bb > aa)
		{
			bb = aa;
		}

		float c = 0;
		float c2 = (aa * aa - bb * bb) / (aa * aa);

		for(int j = 0; j <= 900; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			float f = j * dp + dp2;
			c = c + sqrt(1 - c2 * sin(f) * sin(f)) * dp;
		}

		c = c * 4 * aa;
		aas0 = aas0 + ss;
		aa = (pi - 2) * (pi - 2);
		bb = 4 * (pi - 2) - pi / 4 * elth * elth / ss;
		int cc = 4;
		rr = (-bb - sqrt(bb * bb - 4 * aa * cc)) / (2 * aa);
		float dom1 = doms * dy;
		float dom2 = sqrt(4 * ss / (pi * rr));

		if(ii == iam)
		{
			rrm = rr;
			arm = dom2;
			grm = igg1 * pi / 180;
		}

		ra20 = ra20 + rr * ss;
		idm = (int)(doms + 0.5);

		if(dom1 > domg1)
		{
			domg1 = dom1;
		}

		if(idm < 1)
		{
			idm = 1;
		}

		if(idm > ma1)
		{
			ma1 = idm;
		}
        if (idm > maindex)
        {
			idm = maindex;
			zq[1 * maindex * 51 + idm * 51 + lp0] = 0;
        }
		zq[1 * maindex * 51 + idm * 51 + lp0] = zq[1 * maindex * 51 + idm * 51 + lp0] + hpn[ii];
		idm = (int)(dom2 / dy + 0.5);

		if(dom2 > domg2)
		{
			domg2 = dom2;
		}

		if(idm < 1)
		{
			idm = 1;
		}

		if(idm > ma2)
		{
			ma2 = idm;
		}
        if (idm > maindex)
        {
			idm = maindex;
			zq[2 * maindex * 51 + idm * 51 + lp0] = 0;
        }
		zq[2 * maindex * 51 + idm * 51 + lp0] = zq[2 * maindex * 51 + idm * 51 + lp0] + hpn[ii];
		eth[lp0] = eth[lp0] + elth;
	}

	ra2 = ra2 + ra20;
	aas = aas + aas0;
	float poa0 = eth[lp0] / aas0;
	float rr0 = ra20 / aas0;
	float aa0 = (8 + 4 * (pi - 2) * rr0) / (pi * rr0 * poa0);
	float ayy = aa0 / dy;
	float dd0 = 0, zq1 = 0, zq2 = 0;

	if(ayy < 1)
	{
		dd0 = zq[2 * maindex * 51 + 1 * 51 + lp0] / np0;
	}
	else
	{
		zq1 = 0;
		zq2 = zq[2 * maindex * 51 + 1 * 51 + lp0] / np0;

		for(int l = 2; l <= ma2; l++)
		{
			zq1 = zq2;
			zq2 = zq2 + zq[2 * maindex * 51 + 1 * 50 + lp0] / np0;

			if(ayy >= (l - 1) && ayy <= 1)
			{
				dd0 = zq1 + (zq2 - zq1) * (ayy - l + 1);
				break;
			}
		}
	}

	ma = ma1;

	if(ma < ma2)
	{
		ma = ma2;
	}

	zw[lp0] = dy;
	la[lp0] = ma;
	float amp = 0, ehm = 0;

	for(lp0 = 1; lp0 <= lp; lp0++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		amp = amp + ap[lp0];
		ehm = ehm + eth[lp0];
	}

	float poa = ehm / aas;
	ma = 0;

	for(lp0 = 1; lp0 <= lp; lp0++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int l = 1; l < la[lp0]; l++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			ll = ma + l;
			if (l > maindex)
			{
				l = maindex;
				zp[1][ll] = zq[1 * maindex * 51 + l * 51 + lp0] * ap[lp0] / amp;
				zs[1][ll] = zw[lp0] * l;
				zp[2][ll] = zq[2 * maindex * 51 + l * 51 + lp0] * ap[lp0] / amp;
				zs[2][ll] = zw[lp0] * l;
				zq[1 * maindex * 51 + l * 50 + lp0] = 0;
				zq[2 * maindex * 51 + l * 50 + lp0] = 0;
			}
			zp[1][ll] = zq[1 * maindex * 51 + l * 51 + lp0] * ap[lp0] / amp;
			zs[1][ll] = zw[lp0] * l;
			zp[2][ll] = zq[2 * maindex * 51 + l * 51 + lp0] * ap[lp0] / amp;
			zs[2][ll] = zw[lp0] * l;
			zq[1 * maindex * 51 + l * 50 + lp0] = 0;
			zq[2 * maindex * 51 + l * 50 + lp0] = 0;
		}

		ma = ma + la[lp0];
	}

	mb = ma;

	while(1)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		ij = 0;

		for(int i = 2; i <= mb; i++)
		{
			if(zs[1][i] < zs[1][i - 1])
			{
				ij = 1;
				float zzs = zs[1][i];
				float zzp = zp[1][i];
				zs[1][i] = zs[1][i - 1];
				zp[1][i] = zp[1][i - 1];
				zs[1][i - 1] = zzs;
				zp[1][i - 1] = zzp;
			}
		}

		if(ij != 0)
		{
			mb = mb - 1;
			continue;
		}
		else
		{
			break;
		}
	}

	mb = 1;

	for(int l = 2; l <= ma; l++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		if(zs[1][l] == zs[1][mb])
		{
			zp[1][mb] = zp[1][mb] + zp[1][l];
		}
		else
		{
			mb = mb + 1;
			zs[1][mb] = zs[1][l];
			zp[1][mb] = zp[1][l];
		}
	}

	ma1 = mb;
	mb = ma;

	while(1)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		ij = 0;

		for(int i = 2; i <= mb; i++)
		{
			if(zs[2][i] < zs[2][i - 1])
			{
				ij = 1;
				float zzs = zs[2][i];
				float zzp = zp[2][i];
				zs[2][i] = zs[2][i - 1];
				zp[2][i] = zp[2][i - 1];
				zs[2][i - 1] = zzs;
				zp[2][i - 1] = zzp;
			}
		}

		if(ij != 0)
		{
			mb = mb - 1;
			continue;
		}
		else
		{
			break;
		}
	}

	mb = 1;

	for(int l = 2; l <= ma; l++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		if(zs[2][l] == zs[2][mb])
		{
			zp[2][mb] = zp[2][mb] + zp[2][l];
		}
		else
		{
			mb = mb + 1;
			zs[2][mb] = zs[2][l];
			zp[2][mb] = zp[2][l];
		}
	}

	ma2 = mb;
	float sdom1 = zp[1][1], sdom2 = 0, domx1 = 0, domx2 = 0;

	if(sdom1 <= 0.5)
	{
		for(int l = 2; l <= ma1; l++)
		{
			sdom2 = sdom1 + zp[1][l];

			if(sdom2 >= 0.5)
			{
				if(sdom2 == sdom1)
				{
					domx1 = zs[1][l - 1];
				}
				else
				{
					domx1 = (zs[1][l] - zs[1][l - 1]) * (0.5 - sdom1) / (sdom2 - sdom1) + zs[1][l - 1];
				}

				break;
			}
			else
			{
				sdom1 = sdom2;
			}
		}
	}
	else
	{
		domx1 = zs[1][1];
	}

	sdom1 = zp[2][1];

	if(sdom1 <= 0.5)
	{
		for(int l = 2; l <= ma2; l++)
		{
			sdom2 = sdom1 + zp[2][l];

			if(sdom2 >= 0.5)
			{
				if(sdom2 == sdom1)
				{
					domx2 = zs[2][l - 1];
				}
				else
				{
					domx2 = (zs[2][l] - zs[2][l - 1]) * (0.5 - sdom1) / (sdom2 - sdom1) + zs[2][l - 1];
				}

				break;
			}
			else
			{
				sdom1 = sdom2;
			}
		}
	}
	else
	{
		domx2 = zs[2][1];
	}

	float a = domx1 / 2;
	rr = 2 / a;
	float bb1 = 0, bb2 = 0, bb = 0;

	if(poa > rr)
	{
		bb1 = 0;
		bb2 = a;
		bb = (bb1 + bb2) / 2;

		while(1)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			bb = (bb1 + bb2) / 2;
			float s = pi * a * bb;
			float c = 0;
			float c2 = (a * a - bb * bb) / (a * a);

			for(int j = 0; j <= 900; j++)
			{
				float f = j * dp + dp2;
				c = c + sqrt(1 - c2 * sin(f) * sin(f)) * dp;
			}

			c = c * 4 * a;
			rr = c / s;

			if(abs(rr - poa) <= 0.00001 || (bb2 - bb1) <= 0.0000001)
			{
				break;
			}

			if(rr < poa)
			{
				bb2 = bb;
			}
			else
			{
				bb1 = bb;
			}
		}
	}
	else
	{
		bb = a;
	}

	//float rbx1=bb/a;
	a = domx2 / 2;
	rr = 2 / a;

	if(poa > rr)
	{
		bb1 = 0;
		bb2 = a;
		bb = (bb1 + bb2) / 2;

		while(1)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			bb = (bb1 + bb2) / 2;
			float s = pi * a * bb;
			float c = 0;
			float c2 = (a * a - bb * bb) / (a * a);

			for(int j = 0; j <= 900; j++)
			{
				float f = j * dp + dp2;
				c = c + sqrt(1 - c2 * sin(f) * sin(f)) * dp;
			}

			c = c * 4 * a;
			rr = c / s;

			if(abs(rr - poa) <= 0.00001 || (bb2 - bb1) <= 0.0000001)
			{
				break;
			}

			if(rr < poa)
			{
				bb2 = bb;
			}
			else
			{
				bb1 = bb;
			}
		}
	}
	else
	{
		bb = a;
	}

	//float rbx2=bb/a;
	float dom0 = 4 / poa;
	float rbz = ra2 / aas;
	float aa1 = dom0 / 2;
	float aa2 = domg2 / 2;

	while(1)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		a = (aa1 + aa2) / 2;
		bb = rbz * a;
		float s = pi * a * bb;
		float c = 0;
		float c2 = (a * a - bb * bb) / (a * a);

		for(int j = 0; j <= 900; j++)
		{
			float f = j * dp + dp2;
			c = c + sqrt(1 - c2 * sin(f) * sin(f)) * dp;
		}

		c = c * 4 * a;
		rr = c / s;

		if(abs(rr - poa) <= 0.00001 || (aa2 - aa1) <= 0.0000001)
		{
			break;
		}

		if(rr < poa)
		{
			aa2 = a;
		}
		else
		{
			aa1 = a;
		}
	}

	//int domz=a*2;
	m = m + m0;

	for(int i = m + 1; i <= mj; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			k0 = (i - 1) * n + j;
			kk = (k0 - 1) * 3 + 54 + (i - 1) * iie;
			*pTmp1 = 255;
			fseek(fp1, kk, 0);
			fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);
			fseek(fp1, kk + 1, 0);
			fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);
			fseek(fp1, kk + 2, 0);
			fwrite((void *)pTmp1, sizeof(unsigned char), 1, fp1);
		}

		for(int j = 1; j <= iie; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			fseek(fp1, kk + 2 + j, 0);
			fwrite(&e[m][j], sizeof(unsigned char), 1, fp1);
		}
	}

	xx = 0;
	float  xy = 0;

	for(int j = 1; j <= 360; j++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		float gg = (j - 1) * 2 * pi / 360;
		zg[j] = zg[j] / np0;
		float x = zg[j] * cos(gg);
		float y = zg[j] * sin(gg);
		xx = xx + x * x;
		xy = xy + x * y;
	}

	float g0 = atan2(xy, xx);

	for(jj = 1; jj <= 24; jj++)
	{
		d[jj] = 0;
	}

	for(int j = 9; j <= 339; j = j + 15)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		jj = (j - 9) / 15 + 2;
		j1 = j;
		j2 = j + 14;

		for(int j0 = j1; j0 <= j2; j0++)
		{
			d[jj] = d[jj] + zg[j0];
		}
	}

	for(int j = 1; j <= 8; j++)
	{
		d[1] = d[1] + zg[j];
	}

	for(int j = 354; j <= 360; j++)
	{
		d[1] = d[1] + zg[j];
	}

	float dm = 0;

	for(int l = 1; l <= 24; l++)
	{
		if(d[l] > dm)
		{
			dm = d[l];
		}
	}

	for(int l = 1; l <= 24; l++)
	{
		d[l] = d[l] / dm;
	}

	for(int i = m1; i <= mj; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			r[i][j] = char(255);
			g[i][j] = char(255);
			b[i][j] = char(255);
		}
	}

	int i01 = m + 85;
	int i02 = i01;
	int i03 = i01;
	int i04 = i01;
	//int j01=85;
	int j02 = 266;
	int j03 = 447;
	int j04 = 628;
	m1 = m + 1;
	int j0 = 0;
	float gg, dd, x0, y0;

	for(int i = m1; i <= mj; i++)    //求所有孔隙发育方向和密度
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		j1 = 363;
		j2 = 531;

		for(int j = j1; j <= j2; j++)
		{
			xx = j - j03;
			yy = i - i03;

			if(xx == 0 && yy == 0)
			{
				gg = 0;
			}
			else
			{
				gg = atan2(yy, xx);
			}

			if(gg < (-pi / 24))
			{
				gg = gg + pi;
			}

			int l = (gg + pi / 24) / (pi / 12) + 1;
			dd = 82 * d[l];
			xy = sqrt(xx * xx + yy * yy);

			if(xy <= dd)
			{
				r[i][j] = char((int)(255 * d[l] + 0.5));
				g[i][j] = char(255 - (int)(255 * d[l] + 0.5));
				b[i][j] = 0;
			}

			if((84 - xy) >= 0 && (84 - xy <= 2))
			{
				r[i][j] = 0;
				g[i][j] = 0;
				b[i][j] = 0;
				j0 = j - 181;
				r[i][j0] = 0;
				g[i][j0] = 0;
				b[i][j0] = 0;
				j0 = j + 181;
				r[i][j0] = 0;
				g[i][j0] = 0;
				b[i][j0] = 0;
			}

			j0 = j + 181;
			xx = j0 - j04;
			yy = i - i04;
			x0 = xx * cos(g0) + yy * sin(g0);
			y0 = yy * cos(g0) - xx * sin(g0);
			xy = sqrt(x0 * x0 + y0 * y0 / (rr0 * rr0));

			if(xy < 82)
			{
				r[i][j0] = 255;
				g[i][j0] = 0;
				b[i][j0] = 0;
			}

			j0 = j - 181;
			xx = j0 - j02;
			yy = i - i02;
			x0 = xx * cos(grm) + yy * sin(grm);
			y0 = yy * cos(grm) - xx * sin(grm);
			xy = sqrt(x0 * x0 + y0 * y0 / (rrm * rrm));

			if(xy < 82)
			{
				r[i][j0] = 255;
				g[i][j0] = 0;
				b[i][j0] = 0;
			}
		}
	}

	float gg1, gg2, id1, id2;

	for(int l = 1; l <= 24; l = l + 2)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		gg1 = l * pi / 12;
		gg2 = (l + 1) * pi / 12;
		id1 = 76;
		id2 = 80;

		for(ll = id1; ll <= 84; ll++)
		{
			int i = ll * sin(gg1) + i02;
			int j = ll * cos(gg1) + j02;
			r[i][j] = char(0);
			g[i][j] = char(0);
			b[i][j] = char(255);
			j0 = j + 181;
			r[i][j0] = char(0);
			g[i][j0] = char(0);
			b[i][j0] = char(255);
			j0 = j + 2 * 181;
			r[i][j0] = char(0);
			g[i][j0] = char(0);
			b[i][j0] = char(255);
		}

		for(ll = id2; ll <= 84; ll++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			int i = ll * sin(gg2) + i02;
			int j = ll * cos(gg2) + j02;
			r[i][j] = char(0);
			g[i][j] = char(0);
			b[i][j] = char(255);
			j0 = j + 181;
			r[i][j0] = char(0);
			g[i][j0] = char(0);
			b[i][j0] = char(255);
			j0 = j + 2 * 181;
			r[i][j0] = char(0);
			g[i][j0] = char(0);
			b[i][j0] = char(255);
		}
	}

	for(int i = m + 1; i <= mj; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= 168; j = j + 167)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			k0 = (i - 1) * n + j;
			kk = (k0 - 1) * 3 + 54 + (i - 1) * iie;
			r[i][j] = char(0);
			g[i][j] = char(0);
			b[i][j] = char(0);
			k0 = (i - 1) * n + j + 1;
			kk = (k0 - 1) * 3 + 54 + (i - 1) * iie;
			r[i][j + 1] = char(0);
			g[i][j + 1] = char(0);
			b[i][j + 1] = char(0);
		}
	}

	jj = mj - m - 1;

	for(int i = m + 1; i <= mj; i = i + jj)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= 169; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			k0 = (i - 1) * n + j;
			kk = (k0 - 1) * 3 + 54 + (i - 1) * iie;
			r[i][j] = char(0);
			g[i][j] = char(0);
			b[i][j] = char(0);
			k0 = i * n + j;
			kk = (k0 - 1) * 3 + 54 + i * iie;
			r[i][j] = char(0);
			g[i][j] = char(0);
			b[i][j] = char(0);
		}
	}

	jj = hpm[iam];
	ll = hpn[iam];
	int im1 = m, im2 = 0, jm1 = n, jm2 = 0;

	for(int l = 1; l  <= ll; l++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		kk = jj + l;
		int i = (hpk[kk] - 1) / n + 1;
		int j = hpk[kk] - (i - 1) * n;

		if(i > im2)
		{
			im2 = i;
		}

		if(i < im1)
		{
			im1 = i;
		}

		if(j > jm2)
		{
			jm2 = j;
		}

		if(j < jm1)
		{
			jm1 = j;
		}
	}

	im0 = (im2 + im1) / 2;
	int jm0 = (jm2 + jm1) / 2;

	for(int i = im1; i <= im2; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = jm1; j <= jm2; j++)
		{
			v[i][j] = 0;
		}
	}

	for(int l = 1; l <= ll; l++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		kk = jj + l;
		int i = (hpk[kk] - 1) / n + 1;
		int j = hpk[kk] - (i - 1) * n;
		v[i][j] = 1;
	}

	rr = arm / (dy * 164);
	int i0 = mj - 84;
	j0 = 85;
	i1 = i0 - (im0 - im1) / rr;
	i2 = i0 + (im2 - im0) / rr;
	j1 = j0 - (jm0 - jm1) / rr;
	j2 = j0 + (jm2 - jm0) / rr;

	if(i1 < mj - 166)
	{
		i1 = mj - 166;
	}

	if(i2 > mj - 2)
	{
		i2 = mj - 2;
	}

	if(j1 < 3)
	{
		j1 = 3;
	}

	if(j2 > 167)
	{
		j2 = 167;
	}

	for(int i = i1; i <= i2; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = j1; j <= j2; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			ii = im0 + (int)((i - i0) * rr + 0.5);
			jj = jm0 + (int)((j - j0) * rr + 0.5);

			if(v[ii][jj] == 1)
			{
				r[i][j] = char(255);
				g[i][j] = char(0);
				b[i][j] = char(0);
			}
		}
	}

	for(int i = m + 1; i <= mj; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			k0 = (i - 1) * n + j;
			kk = (k0 - 1) * 3 + 54 + (i - 1) * iie;
			fseek(fp1, kk, 0);
			fwrite(&b[i][j], sizeof(unsigned char), 1, fp1);
			fseek(fp1, kk + 1, 0);
			fwrite(&g[i][j], sizeof(unsigned char), 1, fp1);
			fseek(fp1, kk + 2, 0);
			fwrite(&r[i][j], sizeof(unsigned char), 1, fp1);
		}

		for(int j = 1; j <= iie; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			fseek(fp1, kk + 2 + j, 0);
			fwrite(&e[m][j], sizeof(unsigned char), 1, fp1);
		}
	}

	fclose(fp0);
	fclose(fp1);

	//qWarning("m=%d,n=%d,ma=%d,mb=%d\n",m,n,ma,mb);
	for(int i = 0; i <= m_new; i++)
	{
		delete []r[i];
		delete []g[i];
		delete []b[i];
		delete []e[i];
		delete []v[i];
	}

	delete []r;
	delete []g;
	delete []b;
	delete []e;
	delete []v;

	for(int i = 0; i <= 2; i++)
	{
		delete []zp[i];
		delete []zs[i];
	}

	delete []zp;
	delete []zs;
	delete []ri;
	delete []gj;
	delete []bk;
	delete []rgb;
	delete []hpn;
	delete []hpm;
	delete []hpo;
	delete []hpk;
	delete []zq;
	delete pTmp1;
	delete pTmp2;
	return true;
}

/*****************************************************************
* Purpose: 孔隙结构参数计算算法.
* Parameter: pcPicname | char * | 待扫描的岩样图片名.
*            fDrm | float | 岩样图片中除去备注说明处的宽度比列，实际扫描图片为实际宽度的fDrm倍.
*			 ParaTex | char * | 计算的孔隙结构如孔隙个数、单孔隙周长、面积、长宽比等存入的文本，以备入数据库
*            bThread | bool | 是否有中间过程点击取消返回标识
* Return: returnType || the return value’s meaning.
* Throws: [List exceptions here] || [Why each exception is thrown]
* Note: make some note.
* Others: add some other notes.
*****************************************************************/
bool rsGetElectronMicroscopePara(const char *pcPicname, float fDrm, const char *ParaTxt, bool &bThread)
{
	FILE *fp0 = fopen(pcPicname, "r");   //读一个薄片图像

	if(!fp0)
	{
		printf("read picture failed, in function rsInitInterfacePictruePara");
		return false;
	}

	FILE *fp1 = fopen(ParaTxt, "w");  //扫描分析后的孔隙结构数据写入相应文件

	if(!fp1)
	{
		printf("write Txt failed, in function rsInitInterfacePictruePara");
		return false;
	}

	unsigned char ucA1[15];
	unsigned char ucA2[41];
	int ia1[15];
	int ia2[41];
	int jf[6];
	int jh[12];
	float qk1[1002];
	float qk2[1002];
	float qk3[10002];
	float qk4[1002];
	int mm = 0, nn = 0, mn = 0/*,ll=0*/, m = 0, n = 0, mkp = 0;
	int m0 = 0, mn0 = 0;
	int k0 = 0, kk = 0;
	mkp = mkp + 1;

	for(int i = 4; i < 12; i++)             //扫描图片，将图片对应色素字符串转成相应的ASCII码值
	{
		int l = i + 1;
		fseek(fp0, 14 + i, 0);
		fread(ucA2 + l, sizeof(unsigned char), 1, fp0);
		ia2[l] = ucA2[l];
	}

	nn = ((ia2[8] * 256 + ia2[7]) * 256 + ia2[6]) * 256 + ia2[5];        //计算出要分析图片中孔隙图的长宽，开辟数组用
	mm = ((ia2[12] * 256 + ia2[11]) * 256 + ia2[10]) * 256 + ia2[9];

	if (nn < 0 || nn > 1.0e7 || mm < 0 || mm >  1.0e7)
	{
		return false;
	}

	if(mm > m)
	{
		m = mm;
	}

	if(nn > n)
	{
		n = nn;
	}

	//printf("m=%d,n=%d\n",m,n);
	mn = m * n;
	if (mn < 0 || mn > 1.0e7)
	{
		return false;
	}
	short int *ri = new short int [mn + 1];
	short int *gj = new short int [mn + 1];
	short int *bk = new short int [mn + 1];
	int *lrgb = new int[mn + 1];
	int *rgb = new int[mn + 1];
	int **v = new int *[m + 1];

	for(int i = 0; i <= m; i++)
	{
		v[i] = new int[n + 1];

		for(int j = 0; j <= n; j++)
		{
			v[i][j] = 0;
		}
	}

	unsigned char **r = new unsigned char *[m + 1];
	unsigned char **g = new unsigned char *[m + 1];
	unsigned char **b = new unsigned char *[m + 1];

	for(int i = 0; i <= m; i++)
	{
		r[i] = new unsigned char[n + 1];
		g[i] = new unsigned char[n + 1];
		b[i] = new unsigned char[n + 1];
	}

	int kp = 0;
	kp = kp + 1;

	for(int i = 0; i < 14; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		int l = i + 1;
		fseek(fp0, i, 0);
		fread(ucA1 + l, sizeof(unsigned char), 1, fp0);
		ia1[l] = ucA1[l];
		//printf("l=%d,ia1[l]=%d\n",l,ia1[l]);
	}

	for(int i = 14; i < 54; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		int l = i + 1;
		fseek(fp0, i, 0);
		fread(ucA2 + l - 14, sizeof(unsigned char), 1, fp0);
		ia2[l - 14] = ucA2[l - 14];
		//printf("l=%d,ia2[l]=%d\n",l,ia2[l-14]);
	}
    
	//-------------->根据薄片图片中的色素字符串得到的ASCII码值计算相应分析孔隙结构的参数
	jf[1] = ia1[2] * 256 + ia1[1];
	jf[2] = ((ia1[6] * 256 + ia1[5]) * 256 + ia1[4]) * 256 + ia1[3];
	jf[3] = ia1[8] * 256 + ia1[7];
	jf[4] = ia1[10] * 256 + ia1[9];
	jf[5] = ((ia1[14] * 256 + ia1[13]) * 256 + ia1[12]) * 256 + ia1[11];
	/*for (int i=1;i<=5;i++)
	{
		printf("i=%d,jf[i]=%d\n",i,jf[i]);
	}*/
	jh[1] = ((ia2[4] * 256 + ia2[3]) * 256 + ia2[2]) * 256 + ia2[1];
	jh[2] = ((ia2[8] * 256 + ia2[7]) * 256 + ia2[6]) * 256 + ia2[5];
	jh[3] = ((ia2[12] * 256 + ia2[11]) * 256 + ia2[10]) * 256 + ia2[9];
	jh[4] = ia2[14] * 256 + ia2[13];
	jh[5] = ia2[16] * 256 + ia2[15];
	jh[6] = ((ia2[20] * 256 + ia2[19]) * 256 + ia2[18]) * 256 + ia2[17];
	jh[7] = ((ia2[24] * 256 + ia2[23]) * 256 + ia2[22]) * 256 + ia2[21];
	jh[8] = ((ia2[28] * 256 + ia2[27]) * 256 + ia2[26]) * 256 + ia2[25];
	jh[9] = ((ia2[32] * 256 + ia2[31]) * 256 + ia2[30]) * 256 + ia2[29];
	jh[10] = ((ia2[36] * 256 + ia2[35]) * 256 + ia2[34]) * 256 + ia2[33];
	jh[11] = ((ia2[40] * 256 + ia2[39]) * 256 + ia2[38]) * 256 + ia2[37];
	/*for (int i=1;i<=11;i++)
	{
		printf("i=%d,jh[i]=%d\n",i,jh[i]);
	}*/
	m = jh[3];
	n = jh[2];
	m0 = m * fDrm;
	mn = m * n;
	mn0 = m0 * n;

	for(int i = 1; i <= m; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			k0 = (i - 1) * n + j;
			kk = (k0 - 1) * 3 + 54;
			fseek(fp0, kk, 0);
			fread(&b[i][j], sizeof(unsigned char), 1, fp0);
			bk[k0] =  b[i][j];
			fseek(fp0, kk + 1, 0);
			fread(&g[i][j], sizeof(unsigned char), 1, fp0);
			gj[k0] = g[i][j];
			fseek(fp0, kk + 2, 0);
			fread(&r[i][j], sizeof(unsigned char), 1, fp0);
			ri[k0] = r[i][j];
			//printf("i=%d,j=%d,ri[k0]=%d,gj[k0]=%d,bk[k0]=%d\n",i,j,ri[k0],gj[k0],bk[k0]);
		}
	}

	int m1 = m0 + 1;

	//int lv=0;
	for(int i = m1; i <= m; i++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		for(int j = 1; j <= n; j++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			v[i - m0][j] = 0;
		}
	}

	int rgb1 = 0, rgb2 = 0, rgb0 = 0, nrgb = 0, ll0 = 0;
	float rrr, drgb;
    //------------->根据得到的孔隙结构所需的参数得到孔隙个数，孔隙长宽比等并保存到文件中
	for(int nh = 1; nh <= 7; nh++)
	{
		if(!bThread)
		{
			break;//the thread is cancel.
		}

		if(nh == 1)
		{
			//printf("calculate rgb,kp=%d,mkp=%d\n",1,kp,mkp);
			fprintf(fp1, "%drgb\n", nh);
			rgb1 = 256;
			rgb2 = 0;

			for(k0 = mn0 + 1; k0 <= mn; k0++)
			{
				rgb[k0] = ri[k0] * gj[k0] + ri[k0] * bk[k0] + gj[k0] * bk[k0];
				rrr = (float)rgb[k0];
				rrr = sqrt(rrr / 3.0f);
				rgb[k0] = (int)(rrr + 0.5);

				if(rgb[k0] > rgb2)
				{
					rgb2 = rgb[k0];
				}

				if(rgb[k0] < rgb1)
				{
					rgb1 = rgb[k0];
				}
			}
		}

		if(nh == 2)
		{
			//printf("calculate rg,kp=%d,mkp=%d\n",2,kp,mkp);
			fprintf(fp1, "%drg\n", nh);
			rgb1 = 256;
			rgb2 = 0;

			for(k0 = mn0 + 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = ri[k0] * gj[k0];
				rrr = (float)rgb[k0];
				rrr = sqrt(rrr);
				rgb[k0] = (int)(rrr);

				if(rgb[k0] > rgb2)
				{
					rgb2 = rgb[k0];
				}

				if(rgb[k0] < rgb1)
				{
					rgb1 = rgb[k0];
				}
			}
		}

		if(nh == 3)
		{
			//printf("calculate rb,kp=%d,mkp=%d\n",3,kp,mkp);
			fprintf(fp1, "%drb\n", nh);
			rgb1 = 256;
			rgb2 = 0;

			for(k0 = mn0 + 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = ri[k0] * bk[k0];
				rrr = (float)rgb[k0];
				rrr = sqrt(rrr);
				rgb[k0] = (int)(rrr);

				if(rgb[k0] > rgb2)
				{
					rgb2 = rgb[k0];
				}

				if(rgb[k0] < rgb1)
				{
					rgb1 = rgb[k0];
				}
			}
		}

		if(nh == 4)
		{
			//printf("calculate gb,kp=%d,mkp=%d\n",4,kp,mkp);
			fprintf(fp1, "%dgb\n", nh);
			rgb1 = 256;
			rgb2 = 0;

			for(k0 = mn0 + 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = gj[k0] * bk[k0];
				rrr = (float)rgb[k0];
				rrr = sqrt(rrr);
				rgb[k0] = (int)(rrr);

				if(rgb[k0] > rgb2)
				{
					rgb2 = rgb[k0];
				}

				if(rgb[k0] < rgb1)
				{
					rgb1 = rgb[k0];
				}
			}
		}

		if(nh == 5)
		{
			//printf("calculate r,kp=%d,mkp=%d\n",5,kp,mkp);
			fprintf(fp1, "%dr\n", nh);
			rgb1 = 256;
			rgb2 = 0;

			for(k0 = mn0 + 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = ri[k0];

				if(rgb[k0] > rgb2)
				{
					rgb2 = rgb[k0];
				}

				if(rgb[k0] < rgb1)
				{
					rgb1 = rgb[k0];
				}
			}
		}

		if(nh == 6)
		{
			//printf("calculate g,kp=%d,mkp=%d\n",6,kp,mkp);
			fprintf(fp1, "%dg\n", nh);
			rgb1 = 256;
			rgb2 = 0;

			for(k0 = mn0 + 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = gj[k0];

				if(rgb[k0] > rgb2)
				{
					rgb2 = rgb[k0];
				}

				if(rgb[k0] < rgb1)
				{
					rgb1 = rgb[k0];
				}
			}
		}

		if(nh == 7)
		{
			//printf("calculate b,kp=%d,mkp=%d\n",7,kp,mkp);
			fprintf(fp1, "%db\n", nh);
			rgb1 = 256;
			rgb2 = 0;

			for(k0 = mn0 + 1; k0 <= mn; k0++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				rgb[k0] = bk[k0];

				if(rgb[k0] > rgb2)
				{
					rgb2 = rgb[k0];
				}

				if(rgb[k0] < rgb1)
				{
					rgb1 = rgb[k0];
				}
			}
		}

		int ll = 0;

		for(ll = 1; ll <= 1001; ll++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			qk1[ll] = 0;
			qk2[ll] = 0;
			qk3[ll] = 0;
			qk4[ll] = 0;
		}

		rgb0 = rgb2 - rgb1 + 1;

		if(rgb0 > 10001)
		{
			nrgb = 10001;
		}
		else
		{
			nrgb = rgb0;
		}

		drgb = (float)rgb0 / (float)nrgb;

		for(k0 = mn0 + 1; k0 <= mn; k0++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			ll = (int)((rgb[k0] - rgb1 + 1) * drgb + 0.5);

			if(ll < 1)
			{
				ll = 1;
			}

			if(ll > nrgb)
			{
				ll = nrgb;
			}

			qk1[ll] = qk1[ll] + 1;
		}

		for(ll = 1; ll <= nrgb; ll++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			qk1[ll] = qk1[ll] / (float)(mn - mn0);
		}

		qk2[1] = qk1[1];

		for(ll = 2; ll <= nrgb; ll++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			qk2[ll] = qk1[ll] + qk2[ll - 1];
		}

		qk2[1] = 0;
		qk2[nrgb] = 1;
		qk3[1] = 0;
		ll0 = 2;

		for(ll = 2; ll <= 1001; ll++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			float ph = (float)(ll - 1) / 1000.0f;

			for(int jj = ll0; jj <= nrgb; jj++)
			{
				if(!bThread)
				{
					break;//the thread is cancel.
				}

				if(qk2[jj] >= ph && qk2[jj - 1] <= ph)
				{
					float dqk = qk2[jj] - qk2[jj - 1];

					if(dqk != 0)
					{
						qk3[ll] = (jj - 1 + (ph - qk2[jj - 1]) / dqk - 1) / (float)(nrgb - 1);
					}
					else
					{
						qk3[ll] = (float)(jj - 2) / (float)(nrgb - 1);
					}

					ll0 = jj;
					break;
				}
			}
		}

		qk4[1] = 0;

		for(ll = 2; ll <= 1001; ll++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			qk4[ll] = qk3[ll] - qk3[ll - 1];
		}

		for(ll = 1; ll <= 1001; ll++)
		{
			if(!bThread)
			{
				break;//the thread is cancel.
			}

			fprintf(fp1, "%d\t%f\t%f\t%f\t%f\t%f\n", ll, qk1[ll], qk2[ll], float(ll - 1) / 1000.f, qk3[ll], qk4[ll]);
		}
	}

	fclose(fp0);
	fclose(fp1);

	for(int i = 0; i <= m; i++)
	{
		delete []r[i];
		delete []g[i];
		delete []b[i];
		delete []v[i];
	}

	delete []r;
	delete []g;
	delete []b;
	delete []v;
	delete []ri;
	delete []gj;
	delete []bk;
	delete []rgb;
	delete []lrgb;
	return true;
}