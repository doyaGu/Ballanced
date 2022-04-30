#include "GameInfo.h"

#include <string.h>

CGameInfo::CGameInfo() : next(NULL), hkRoot(HKEY_CURRENT_USER)
{
    strcpy(regSubkey, "Software\\Terratools\\WorldMap\\Powerball");
}