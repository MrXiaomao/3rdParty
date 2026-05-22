/*
 * @Author: MrPan
 * @Date: 2025-07-16 14:48:52
 * @LastEditors: Maoxiaoqing
 * @LastEditTime: 2025-07-18 14:50:44
 * @Description: 请填写简介
 */
#pragma once

#include <windows.h>
#include <dbghelp.h>
#include <QMessageBox>
#include <QString>

// 函数指针定义
typedef BOOL (WINAPI *MiniDumpWriteDumpPtr)(
    HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
    );

// ✅ 优化1：使用更丰富的Dump类型，保留更多调试信息
static void miniDumpWriteDump(HANDLE hProcess, DWORD ProcessId, HANDLE hDumpFile,
                              CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
                              CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam) {
    HMODULE module = LoadLibraryW(L"Dbghelp.dll");
    if (module) {
        MiniDumpWriteDumpPtr MiniDumpWriteDump = reinterpret_cast<MiniDumpWriteDumpPtr>(
            GetProcAddress(module, "MiniDumpWriteDump")
            );
        if (MiniDumpWriteDump) {
            // 保留完整内存信息、线程信息、模块信息，确保符号能正确解析
            MINIDUMP_TYPE dumpType = static_cast<MINIDUMP_TYPE>(
//                MiniDumpWithFullMemory |  // 关键：保留完整内存信息，栈就存在这里
//                MiniDumpWithFullMemoryInfo |
//                MiniDumpWithThreadInfo |
//                MiniDumpWithProcessThreadData |
//                MiniDumpWithUnloadedModules |
//                MiniDumpWithIndirectlyReferencedMemory
                MiniDumpNormal |                  // 基础Dump
                MiniDumpWithDataSegs |            // 数据段
                MiniDumpWithFullMemoryInfo |      // 内存信息
                MiniDumpWithThreadInfo |          // 线程信息
                MiniDumpWithProcessThreadData |   // 线程栈数据
                MiniDumpWithIndirectlyReferencedMemory // 关联内存数据
                );
            MiniDumpWriteDump(hProcess, ProcessId, hDumpFile, dumpType,
                              ExceptionParam, nullptr, CallbackParam);
        }
        FreeLibrary(module);
    }
}

BOOL CALLBACK MiniDumpRoutineCallback(PVOID, const PMINIDUMP_CALLBACK_INPUT input,
                                      PMINIDUMP_CALLBACK_OUTPUT output) {
    if (!input || !output) return FALSE;

    switch (input->CallbackType) {
    case IncludeModuleCallback:
    case IncludeThreadCallback:
    case ThreadCallback:
    case ThreadExCallback:
        return TRUE;
    case ModuleCallback:
        // 仅保留被引用的模块，减小dmp体积
        if (!(output->ModuleWriteFlags & ModuleReferencedByMemory)) {
            output->ModuleWriteFlags &= ~ModuleWriteModule;
        }
        return TRUE;
    default:
        return FALSE;
    }
}

// ✅ 优化2：dmp文件保存到用户文档目录，避免权限不足无法写入
void WriteDump(EXCEPTION_POINTERS *exp, const std::wstring &path) {
    // 打开文件时添加 FILE_FLAG_WRITE_THROUGH 标志，强制同步写入
    HANDLE hDumpFile = ::CreateFileW(path.c_str(), GENERIC_WRITE | GENERIC_READ,
                                     FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr);
    if (hDumpFile == INVALID_HANDLE_VALUE) return;

    MINIDUMP_EXCEPTION_INFORMATION info;
    info.ThreadId = ::GetCurrentThreadId();
    info.ExceptionPointers = exp;
    info.ClientPointers = FALSE;

    MINIDUMP_CALLBACK_INFORMATION mci;
    mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE) MiniDumpRoutineCallback;
    mci.CallbackParam = nullptr;

    //miniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, &info, &mci);
    // 去掉回调函数，避免回调逻辑影响Dump生成
    miniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, &info, nullptr);

    ::FlushFileBuffers(hDumpFile);
    ::CloseHandle(hDumpFile);

    // 写入完成后再次校验文件大小
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fileInfo)) {
        // 小于100KB说明写入失败，自动重试一次
        if (fileInfo.nFileSizeLow < 100 * 1024) {
            ::DeleteFileW(path.c_str());
            // 重试一次生成
            HANDLE hRetry = ::CreateFileW(path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hRetry != INVALID_HANDLE_VALUE) {
                miniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hRetry, &info, nullptr);
                ::FlushFileBuffers(hRetry);
                ::CloseHandle(hRetry);
            }
        }
    }
}

LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS *exp) {
    // 生成时间命名的dmp文件
    WCHAR timeStr[32] = {0};
    SYSTEMTIME time;
    GetLocalTime(&time);
    swprintf_s(timeStr, L"%4d%02d%02d%02d%02d%02d",
               time.wYear, time.wMonth, time.wDay,
               time.wHour, time.wMinute, time.wSecond);

    // 保存到用户文档目录，确保写入权限
    QString dumpPath = QString("crash_%1.dmp").arg(QString::fromWCharArray(timeStr));
    WriteDump(exp, dumpPath.toStdWString());

    // 友好提示
    QMessageBox::critical(nullptr, "程序异常",
        QString("程序遇到意外错误即将退出，崩溃日志已保存到：\n%1").arg(dumpPath),
                          QMessageBox::Ok);

    return EXCEPTION_EXECUTE_HANDLER;
}

// ✅ 优化3：支持Qt多线程异常捕获（默认仅捕获主线程异常）
void InstallExceptionFilter() {
    ::SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    // 可选：Qt5+ 捕获Qt事件循环中的异常
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        if (type == QtFatalMsg) {
            EXCEPTION_POINTERS exp = {0};
            MyUnhandledExceptionFilter(&exp);
        }
    });
}

