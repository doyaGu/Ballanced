#ifndef TERRATOOLS_H
#define TERRATOOLS_H

enum TT_MSG
{
    TT_MSG_NO_GAMEINFO = 0x5F7,
    TT_MSG_CMO_RESTART = 0x5F8,
    TT_MSG_CMO_LOAD = 0x5F9,
    TT_MSG_EXIT_TO_SYS = 0x5FA,
    TT_MSG_EXIT_TO_TITLE = 0x5FB,
    TT_MSG_LIMIT_FPS = 0x5FC,
    TT_MSG_SCREEN_MODE_CHG = 0x5FD,
    TT_MSG_GO_FULLSCREEN = 0x678,
    TT_MSG_STOP_FULLSCREEN = 0x679,
};

#define TERRATOOLS_GUID CKGUID(0x56495254, 0x4F4F4C53)

#endif // TERRATOOLS_H