#include "Utils.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <sys/stat.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <ShlObj.h>

namespace utils
{
    static struct stat g_Stat;

    bool IsFileExist(const char *file)
    {
        if (!file || strcmp(file, "") == 0)
            return false;

        memset(&g_Stat, 0, sizeof(struct stat));
        return stat(file, &g_Stat) == 0 && (g_Stat.st_mode & S_IFREG);
    }

    bool IsDirectoryExist(const char *dir)
    {
        if (!dir || strcmp(dir, "") == 0)
            return false;

        memset(&g_Stat, 0, sizeof(struct stat));
        return stat(dir, &g_Stat) == 0 && (g_Stat.st_mode & S_IFDIR);
    }

    bool IsAbsolutePath(const char *path)
    {
        if (!path || strcmp(path, "") == 0)
            return false;

        if (strlen(path) < 2 || !isalpha(path[0]) || path[1] != ':')
            return false;

        return true;
    }

    bool GetAbsolutePath(const char *path, char *buffer, int size)
    {
        if (!path || strcmp(path, "") == 0)
            return false;

        if (IsAbsolutePath(path))
        {
            strncpy(buffer, path, size);
        }
        else
        {
            size_t len = ::GetCurrentDirectoryA(size, buffer);
            len = size - 1 - len;
            strncat(buffer, "\\", len);
            --len;
            strncat(buffer, path, len);
        }

        if (path[strlen(path) - 1] != '\\')
            strncat(buffer, "\\", size - 1 - strlen(buffer));

        return true;
    }

    bool CheckVirtoolsDlls(const char *path)
    {
        if (!path || strcmp(path, "") == 0)
            return false;

        static const char *dlls[3] = {"CK2.dll", "CKZlib.dll", "VxMath.dll"};
        char szPath[MAX_PATH];
        for (int i = 0; i < 3; ++i)
        {
            _snprintf(szPath, MAX_PATH, "%s%s", path, dlls[i]);
            if (!IsFileExist(szPath))
                return false;
        }
        return true;
    }

    bool GetDefaultRootPath(char *path, int size)
    {
        char szPath[MAX_PATH];
        char drive[4];
        char dir[MAX_PATH];
        char filename[MAX_PATH];
        ::GetModuleFileNameA(NULL, szPath, MAX_PATH);
        _splitpath(szPath, drive, dir, filename, NULL);

        _snprintf(szPath, size, "%s%s", drive, dir);
        if (CheckVirtoolsDlls(szPath))
        {
            _snprintf(path, size, "%s%s..\\", drive, dir);
            return true;
        }

        _snprintf(szPath, size, "%s%sBin\\", drive, dir);
        if (CheckVirtoolsDlls(szPath))
        {
            _snprintf(path, size, "%s%s", drive, dir);
            return true;
        }

        return false;
    }

    bool BrowsePath(const char *title, char *path)
    {
        BROWSEINFOA bInfo = {0};
        bInfo.hwndOwner = ::GetForegroundWindow();
        bInfo.lpszTitle = title;
        bInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI | BIF_UAHINT | BIF_NONEWFOLDERBUTTON;

        LPITEMIDLIST lpDlist;
        lpDlist = ::SHBrowseForFolderA(&bInfo);
        if (!lpDlist)
            return false;

        ::SHGetPathFromIDListA(lpDlist, path);
        return true;
    }

    bool CreatePathTree(const char *path)
    {
        if (!path || strcmp(path, "") == 0)
            return false;

        const size_t sz = strlen(path);
        if (sz <= 2)
            return false;

        char *pt = new char[sz + 1];
        memcpy(pt, path, sz + 1);

        for (char *pch = &pt[3]; *pch != '\0'; ++pch)
        {
            if (*pch != '/' && *pch != '\\')
                continue;
            *pch = '\0';
            if (::GetFileAttributesA(pt) == -1)
                if (!::CreateDirectoryA(pt, NULL))
                    break;
            *pch = '\\';
        }

        delete[] pt;
        return true;
    }

    int CharToWchar(LPCSTR pCharStr, LPWSTR pWCharStr, INT iSize)
    {
        return ::MultiByteToWideChar(CP_ACP, 0, pCharStr, -1, pWCharStr, iSize);
    }

    int WcharToChar(LPCWSTR pWCharStr, LPSTR pCharStr, INT iSize)
    {
        return ::WideCharToMultiByte(CP_ACP, 0, pWCharStr, -1, pCharStr, iSize, NULL, NULL);
    }
}
