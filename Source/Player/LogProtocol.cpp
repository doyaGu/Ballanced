#include "StdAfx.h"

#include "LogProtocol.h"

#include <io.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

CLogProtocol &CLogProtocol::GetInstance()
{
    static CLogProtocol lp;
    return lp;
}

void CLogProtocol::Open(const char *filename, const char *path, bool overwrite)
{
    FILE *file;

    sprintf(m_FullPath, "%sLog%s.csv", path, filename);

    if (overwrite)
    {
        file = fopen(m_FullPath, "w");
    }
    else
    {
        file = fopen(m_FullPath, "a");
    }

    if (file)
    {
        fprintf(file, "\nLog Messages:\n\n");
        fclose(file);
        strcpy(m_FileName, filename);
        strcpy(m_Path, path);
    }
}

void CLogProtocol::Write(const char *msg)
{
    FILE *file;
    char buffer[256];
    char achDate[12];
    char achTime[12];

    if (m_FileName[0] == '\0')
        return;

    _strdate(achDate);
    _strtime(achTime);
    sprintf(buffer, "%s", msg);

    file = fopen(m_FullPath, "a");
    if (file)
    {
        fprintf(file, "%s, %s, %s\n", buffer, achDate, achTime);
        fclose(file);
    }
}

CLogProtocol::CLogProtocol()
{
}
