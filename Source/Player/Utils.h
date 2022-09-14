#ifndef PLAYER_UTILS_H
#define PLAYER_UTILS_H

#include "wchar.h"

namespace utils
{
    bool IsFileExist(const char *file);

    bool IsDirectoryExist(const char *dir);

    bool IsAbsolutePath(const char *path);

    bool GetAbsolutePath(const char *path, char *buffer, int size);

    bool CheckVirtoolsDlls(const char *path);

    bool GetDefaultRootPath(char *path, int size);

    bool BrowsePath(const char *title, char *path);

    bool CreatePathTree(const char *path);

    int CharToWchar(const char *charStr, wchar_t *wcharStr, int size);

    int WcharToChar(const wchar_t *wcharStr, char *charStr, int size);
}

#endif // PLAYER_UTILS_H
