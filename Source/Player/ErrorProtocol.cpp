#include "ErrorProtocol.h"

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#include <io.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

CErrorProtocol &CErrorProtocol::GetInstance()
{
    static CErrorProtocol ep;
    return ep;
}

void CErrorProtocol::Open(const char *filename, const char *path, bool overwrite)
{
    FILE *file;

    sprintf(m_FullPath, "%sError%s.csv", path, filename);
    
    if (overwrite)
        file = fopen(m_FullPath, "w");
    else
        file = fopen(m_FullPath, "a");

    if (file)
    {
        if (overwrite)
            fprintf(file, "\nSystem Name, Module Name, Function, Error Status, Date, Time\n\n");
        fclose(file);
        strcpy(m_FileName, filename);
        strcpy(m_Path, path);
    }
}

void CErrorProtocol::Write(const char *srcfile, const char *func, const char *msg, bool showMsgBox)
{
    FILE *file;
    char buffer[256];
    char achDate[12];
    char achTime[12];

    if (m_FileName[0] == '\0')
        return;

    _strdate(achDate);
    _strtime(achTime);
    sprintf(buffer, "%s, %s, %s, %s", m_FileName, srcfile, func, msg);

    if (showMsgBox)
		::MessageBoxA(NULL, buffer, "Error", MB_ICONERROR);

    file = fopen(m_FullPath, "a");
    if (file)
    {
        fprintf(file, "%s, %s, %s\n", buffer, achDate, achTime);
        fclose(file);
    }
}

CErrorProtocol::CErrorProtocol()
{
}