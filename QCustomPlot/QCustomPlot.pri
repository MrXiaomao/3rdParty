QT       += printsupport

# INCLUDEPATH += -L $$PWD
INCLUDEPATH += $$PWD
DEFINES += QXT_STATIC

HEADERS += $$PWD/qcustomplot.h \
    $$PWD/QCPOverlayWidget.h \
    $$PWD/QCPWaveformStatsPanel.h \
    $$PWD/WaveformStats.h \
    $$PWD/qcustomplothelper.h \
    $$PWD/qxtcheckcombobox.h \
    $$PWD/qxtcheckcombobox_p.h

SOURCES += $$PWD/qcustomplot.cpp \
    $$PWD/qcustomplothelper.cpp \
    $$PWD/qxtcheckcombobox.cpp
