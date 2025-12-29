HEADERS += $$PWD/src/H5AbstractDs.h \
    $$PWD/src/H5Alltypes.h \
    $$PWD/src/H5ArrayType.h \
    $$PWD/src/H5AtomType.h \
    $$PWD/src/H5Attribute.h \
    $$PWD/src/H5Classes.h \
    $$PWD/src/H5CommonFG.h \
    $$PWD/src/H5CompType.h \
    $$PWD/src/H5Cpp.h \
    $$PWD/src/H5CppDoc.h \
    $$PWD/src/H5DaccProp.h \
    $$PWD/src/H5DataSet.h \
    $$PWD/src/H5DataSpace.h \
    $$PWD/src/H5DataType.h \
    $$PWD/src/H5DcreatProp.h \
    $$PWD/src/H5DxferProp.h \
    $$PWD/src/H5EnumType.h \
    $$PWD/src/H5Exception.h \
    $$PWD/src/H5FaccProp.h \
    $$PWD/src/H5FcreatProp.h \
    $$PWD/src/H5File.h \
    $$PWD/src/H5FloatType.h \
    $$PWD/src/H5Group.h \
    $$PWD/src/H5IdComponent.h \
    $$PWD/src/H5Include.h \
    $$PWD/src/H5IntType.h \
    $$PWD/src/H5LaccProp.h \
    $$PWD/src/H5LcreatProp.h \
    $$PWD/src/H5Library.h \
    $$PWD/src/H5Location.h \
    $$PWD/src/H5Object.h \
    $$PWD/src/H5OcreatProp.h \
    $$PWD/src/H5PredType.h \
    $$PWD/src/H5PropList.h \
    $$PWD/src/H5StrType.h \
    $$PWD/src/H5VarLenType.h \

SOURCES += $$PWD/src/H5AbstractDs.cpp \
    $$PWD/src/H5ArrayType.cpp \
    $$PWD/src/H5AtomType.cpp \
    $$PWD/src/H5Attribute.cpp \
    $$PWD/src/H5CommonFG.cpp \
    $$PWD/src/H5CompType.cpp \
    $$PWD/src/H5DaccProp.cpp \
    $$PWD/src/H5DataSet.cpp \
    $$PWD/src/H5DataSpace.cpp \
    $$PWD/src/H5DataType.cpp \
    $$PWD/src/H5DcreatProp.cpp \
    $$PWD/src/H5DxferProp.cpp \
    $$PWD/src/H5EnumType.cpp \
    $$PWD/src/H5Exception.cpp \
    $$PWD/src/H5FaccProp.cpp \
    $$PWD/src/H5FcreatProp.cpp \
    $$PWD/src/H5File.cpp \
    $$PWD/src/H5FloatType.cpp \
    $$PWD/src/H5Group.cpp \
    $$PWD/src/H5IdComponent.cpp \
    $$PWD/src/H5IntType.cpp \
    $$PWD/src/H5LaccProp.cpp \
    $$PWD/src/H5LcreatProp.cpp \
    $$PWD/src/H5Library.cpp \
    $$PWD/src/H5Location.cpp \
    $$PWD/src/H5Object.cpp \
    $$PWD/src/H5OcreatProp.cpp \
    $$PWD/src/H5PredType.cpp \
    $$PWD/src/H5PropList.cpp \
    $$PWD/src/H5StrType.cpp \
    $$PWD/src/H5VarLenType.cpp \

win32: LIBS += -L$$PWD/lib/ -lhdf5
unix:!macx: LIBS += -L$$PWD/lib/unix/ -lhdf5 -lz -lsz
unix:!macx: PRE_TARGETDEPS += $$PWD/lib/unix/libhdf5.a
unix:!macx: PRE_TARGETDEPS += $$PWD/lib/unix/libz.a
unix:!macx: PRE_TARGETDEPS += $$PWD/lib/unix/libsz.a

INCLUDEPATH += $$PWD/src/
INCLUDEPATH += $$PWD/include/
DEPENDPATH += $$PWD/include/

