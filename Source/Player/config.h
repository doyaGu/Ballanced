#ifndef PLAYER_CONFIG_H
#define PLAYER_CONFIG_H

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 5
#define VERSION "0.1.5"
#define DESCRIPTION "Ballance Decompilation"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_BPP 32

#ifndef PLAYER_EXPORT
#ifdef PLAYER_EXPORTS
#define PLAYER_EXPORT __declspec(dllexport)
#else
#define PLAYER_EXPORT
#endif
#endif

#endif // PLAYER_CONFIG_H
