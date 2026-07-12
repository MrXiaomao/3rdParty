QT       += printsupport

# INCLUDEPATH += -L $$PWD
INCLUDEPATH += $$PWD
DEFINES += QXT_STATIC

HEADERS += $$PWD/qcustomplot.h \
    $$PWD/qcustomplothelper.h \
    $$PWD/qxtcheckcombobox.h \
    $$PWD/qxtcheckcombobox_p.h \
    $$PWD/qxtglobal.h \
    $$PWD/qxtnamespace.h
		   		
SOURCES += $$PWD/qcustomplot.cpp \
    $$PWD/qcustomplothelper.cpp \
    $$PWD/qxtcheckcombobox.cpp \
    $$PWD/qxtglobal.cpp
