#include "rsporestructureanalysis.h"
#include <rs.h>
#include <qobject.h>
#include <qstring.h>
#include <rsprocdef.h>
#include <qpixmap.h>
#include<vld.h>

static const char * cross_xpm[] = {
"32 32 5 1",
" 	c white",
".	c Red3",
"X	c Black",
"o	c Tan",
"O	c Thistle",
"                                ",
"                                ",
"  .                             ",
"  X...                          ",
"   XX...                        ",
"    XX ...                      ",
"     XX  ..                     ",
"      XX  .                ooo  ",
"       XX ..         ooooooooo  ",
"        XX .    oooooooooooOOO  ",
"         XX.oooooooooOOOOOOOOO  ",
"     oooooo..oooOOOOOOOOOOOOOO  ",
"  oooooooooX.OOOOOOOOOOOOOOOOO  ",
"  oooOOOOOOO.XOOOOOOOOOOOOOOOO  ",
"  OOOOOOOOOO.XXOOOOOOOOOOOOOOO  ",
"  OOOOOOOOOO..XXOOOOOOOOOOOOOO  ",
"  OOOOOOOOOOO.OXXOOOOOOOOOOOOO  ",
"  OOOOOOOOOOO..OXXOOOOOOOOOOOO  ",
"  OOOOOOOOOOOO.OOXXOOOOOOOOOOO  ",
"  OOOOOOOOOOOO..OOXXOOOOOOOOOO  ",
"  OOOOOOOOOOOOO.OOOXXOOOOOOOOO  ",
"  OOOOOOOOOOOOOO.OOOXXOOOOOooo  ",
"  OOOOOOOOOOOOOOO......ooooooo  ",
"  OOOOOOOOOOOOOOooooooo...o     ",
"  OOOOOOOOOoooooooooo  XX ..    ",
"  OOOooooooooooo        XX ..   ",
"  ooooooooo              XX .   ",
"  ooo                     XX..  ",
"                           XX.  ",
"                            X.  ",
"                                ",
"                                "};
#ifdef __cplusplus
extern "C" {
#endif

    RS_EXPORT bool RS_PROC_IDENTIFY_CALLBACK_NAME(QString &strType, RS_PROC_DATA *pProcData);
    RS_EXPORT int RS_PROC_MAIN(RSUIProject *pUIProject, QWidget *pParent,const QString &strFileName);

#ifdef __cplusplus
}
#endif

bool RS_PROC_IDENTIFY_CALLBACK_NAME(QString &strType, RS_PROC_DATA *pProcData)
{
    if(pProcData==NULL)
        return false;

    strType=RS_PROC_TYPE_GEOLOG_ANALYSIS;
    
	pProcData->m_strName = QObject::tr("PoreStructureAnalysis");
	pProcData->m_strComment = QObject::tr("Application for PoreStructureAnalysis");
    pProcData->m_pPixmap = new QPixmap(cross_xpm);
    return true;
}

int RS_PROC_MAIN(RSUIProject *pUIProject, QWidget *pParent,const QString &strFileName)
{
    if(!RS_CheckLicense())
		return RS_PROC_MAIN_RETURN_OVER;
    if(pUIProject==NULL)
		return RS_PROC_MAIN_RETURN_OVER;
    RSPoreStructureAnalysis *pW = new RSPoreStructureAnalysis(NULL, Qt::WDestructiveClose);
    pW->setLibName(strFileName);
    pW->setProject(pUIProject);
    pW->show();
	QObject::connect(pW, SIGNAL(closeLibrary(const QString &)) , pParent, SLOT(closeLibrary(const QString &)));
    return RS_PROC_MAIN_RETURN_WINDOW;
}
