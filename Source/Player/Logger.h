#ifndef PLAYER_LOGGER_H
#define PLAYER_LOGGER_H

#include "stdarg.h"
#include "stdio.h"

class CLogger
{
public:
    static CLogger &Get();

    ~CLogger();

    void Open(const char *filename, const char *path, bool overwrite);
    void Close();

    void Info(const char *fmt, ...);
    void Warn(const char *fmt, ...);
    void Error(const char *fmt, ...);

private:
    void Log(const char *level, const char *fmt, va_list args);

    CLogger();
    CLogger(const CLogger &);
    CLogger &operator=(const CLogger &);

    FILE *m_File;
};

#endif // PLAYER_LOGGER_H
