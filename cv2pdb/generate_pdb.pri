INCLUDEPATH += $$PWD
HEADERS += $$PWD/app_dmp.h \

# --------------------------
# PDB 生成逻辑
# --------------------------
win32-g++ {
    # --------------------------
    # 全局基础配置（和你原逻辑完全一致，无额外参数）
    # --------------------------
    # 移除LTO参数
    QMAKE_LFLAGS -= -flto
    QMAKE_LFLAGS_RELEASE -= -flto

    # ✅ 全局保留调试信息，禁止剥离
    QMAKE_CXXFLAGS += -g
    QMAKE_CFLAGS += -g
    QMAKE_LFLAGS += -g
    QMAKE_LFLAGS -= -s
    QMAKE_LFLAGS_RELEASE -= -s

    # ✅ Release模式强制配置：覆盖所有默认优化参数，保留调试信息
    CONFIG(release, debug|release) {
        # 完全覆盖默认编译参数，避免-O2和剥离逻辑
        QMAKE_CXXFLAGS = -g -O1 -Wall
        QMAKE_CFLAGS = -g -O1 -Wall
        QMAKE_LFLAGS = -g

        # 确保没有其他配置覆盖我们的参数
        QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CXXFLAGS
        QMAKE_CFLAGS_RELEASE = $$QMAKE_CFLAGS
        QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS
    }

    #QMAKE_PRE_LINK += xcopy "D:\abcdeft.txt" $$DESTDIR /c /h /y /d
    #QMAKE_POST_LINK += xcopy "D:\abcdeft.txt" $$DESTDIR /c /h /y /d

    # 生成最完整的调试信息，禁用压缩和裁剪
    QMAKE_CXXFLAGS += -g2 -gdwarf-4 -fno-omit-frame-pointer
    QMAKE_CFLAGS += -g2 -gdwarf-4 -fno-omit-frame-pointer
    QMAKE_LFLAGS += -g2

    # Release 模式强制保留所有调试信息
    CONFIG(release, debug|release) {
        QMAKE_CXXFLAGS = -g2 -O0 -Wall
        QMAKE_CFLAGS = -g2 -O0 -Wall
        QMAKE_LFLAGS = -g2
    }

    # 单独给qcustomplot.cpp关闭优化，避免编译错误（主要用于防止空函数体编译错误）
    QMAKE_CXXFLAGS_qcustomplot.o = -O0 -g2

    # cv2pdb路径优先从环境变量读取，否则使用本地3rdParty路径
    CV2PDB_PATH = $$(CV2PDB_PATH)
    isEmpty(CV2PDB_PATH) {
        CV2PDB_PATH = "$$PWD/cv2pdb.exe"
    }

    exists($$CV2PDB_PATH) {
        message("Found cv2pdb.exe, will general PDB file.")
        message("Convert command: $$CV2PDB_PATH $$DESTDIR/$${TARGET}.exe $$DESTDIR/$${TARGET}.pdb")

        # 先在编译临时目录生成PDB，再复制到目标目录，避免权限/占用问题
        QMAKE_POST_LINK += $$CV2PDB_PATH $$DESTDIR/$${TARGET}.exe $$DESTDIR/$${TARGET}_pdb.exe
        #QMAKE_POST_LINK += if exist "$$DESTDIR/$${TARGET}_pdb.exe" del /f /q "$$DESTDIR/$${TARGET}_pdb.exe"

        QMAKE_CLEAN += $$shell_path($$OUT_PWD/$${TARGET}.pdb) $$shell_path($$DESTDIR/$${TARGET}.pdb) $$shell_path($$DESTDIR/$${TARGET}_pdb.exe)
    } else {
        message("Not found cv2pdb.exe，skipping PDB. Path: $$CV2PDB_PATH")
    }
}

# --------------------------
# 下面脚本用于批量复制文件
# --------------------------
#FROM_FILES += \

#defineTest(copyToDestDir) {
#    files = $$1
#    dir = $$2

#    win32:dir ~= s,/,\\,g
#    for (file, files) {
#        win32:file ~= s,/,\\,g
#        QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$shell_quote($$file) $$shell_quote($$dir) $$escape_expand(\\n\\t)
#    }

#    export(QMAKE_POST_LINK)
#}

#copyToDestDir($$FROM_FILES, $$DESTDIR/)
