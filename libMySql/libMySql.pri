HEADERS += $$PWD/QMySqlDatabase.h
SOURCES += $$PWD/QMySqlDatabase.cpp

# 添加 mysql 相关库
LIBS += -L$$PWD/lib/ -llibmysql

# 添加 mysql 相关头文件
INCLUDEPATH += $$PWD/include
