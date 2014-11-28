TEMPLATE	= lib
LANGUAGE	= C++
DEFINES += _CRT_SECURE_NO_WARNINGS

CONFIG	+= qt warn_on release

LIBS	+= -lrsbasic -luibasic -lrsuiproject -lrswell -luiwell -lrsuicolorbar -lrsuiutil -lrsutil -lrscrossplot -lrsmessage

INCLUDEPATH	+= ..\..\include\crossplot

HEADERS	+= rsporestructureanalysis.h \
               rssqltablemodel.h\
               rstableeditor.h\
               rsanalyticalmethodofrocksample.h\
               rsthreadcalc.h\
               rscomboboxdelegate.h
	

SOURCES	+= rsporestructureanalysis.cpp \
               rsporestructureanalysisexport.cpp\
               rssqltablemodel.cpp\
               rstableeditor.cpp\
               rsanalyticalmethodofrocksample.cpp\
               rsthreadcalc.cpp\
               rscomboboxdelegate.cpp


FORMS	= rsporestructureanalysis.ui \

	


unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

include(../rstemplate.pri)