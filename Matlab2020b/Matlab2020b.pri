INCLUDEPATH += -L $$PWD

# Matlab SDK库的头文件
win32: LIBS += -L$$PWD/UnfolddingAlgorithm_Gravel/ -lUnfolddingAlgorithm_Gravel
INCLUDEPATH += $$PWD/UnfolddingAlgorithm_Gravel/

# Matlab 运行库的头文件
INCLUDEPATH += $$PWD/v99/extern/include
INCLUDEPATH += $$PWD/v99/extern/include/Win64

# INCLUDEPATH += $$PWD/3rdParty/Matlab2020b/v99/extern/lib/win64/mingw64
# DEPENDPATH += $$PWD/3rdParty/Matlab2020b/v99/extern/lib/win64/mingw64

win32: LIBS += -L$$PWD/v99/extern/lib/win64/mingw64/ -llibmex
win32: LIBS += -L$$PWD/v99/extern/lib/win64/mingw64/ -llibmx
win32: LIBS += -L$$PWD/v99/extern/lib/win64/mingw64/ -llibmat
win32: LIBS += -L$$PWD/v99/extern/lib/win64/mingw64/ -llibeng
win32: LIBS += -L$$PWD/v99/extern/lib/win64/mingw64/ -lmclmcr
win32: LIBS += -L$$PWD/v99/extern/lib/win64/mingw64/ -lmclmcrrt
