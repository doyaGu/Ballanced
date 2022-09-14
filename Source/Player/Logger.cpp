#include "Logger.h"

#include <stdio.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "Windows.h"

CLogger &CLogger::Get()
{
    static CLogger logger;
    return logger;
}

CLogger::~CLogger() {
    Close();
}

void CLogger::Open(const char *filename, const char *path, bool overwrite)
{
    if (m_File) return;

    char fullPath[512];
    sprintf(fullPath, "%s%s.log", path, filename);

    if (overwrite)
        m_File = fopen(fullPath, "w");
    else
        m_File = fopen(fullPath, "a");
}

void CLogger::Close() {
    if (m_File)
        fclose(m_File);
}


void CLogger::Info(const char *fmt, ...) {
    va_list args;
        va_start(args, fmt);
    Log("INFO", fmt, args);
        va_end(args);
}

void CLogger::Warn(const char *fmt, ...) {
    va_list args;
        va_start(args, fmt);
    Log("WARN", fmt, args);
        va_end(args);
}

void CLogger::Error(const char *fmt, ...) {
    va_list args;
        va_start(args, fmt);
    Log("ERROR", fmt, args);
        va_end(args);
}

void CLogger::Log(const char *level, const char *fmt, va_list args)
{
    SYSTEMTIME sys;
    GetLocalTime(&sys);

    if (!m_File)
        return;

    fprintf(m_File, "[%02d/%02d/%d %02d:%02d:%02d.%03d] ",
            sys.wMonth, sys.wDay, sys.wYear,
            sys.wHour, sys.wMinute, sys.wSecond, sys.wMilliseconds);
    fprintf(m_File, "[%s]: ", level);
    vfprintf(m_File, fmt, args);
    fputc('\n', m_File);
    fflush(m_File);
}

CLogger::CLogger() : m_File(NULL) {}
