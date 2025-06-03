#ifndef CONTROLLERS_PARAMS_H
#define CONTROLLERS_PARAMS_H

typedef struct mappedJoy
{
    mappedJoy()
    {
        joy = -1;
        messageType = -1;
        memset(messageName, 0, 32);
    }
    int joy;
    int messageType;
    char messageName[32];
} mappedJoy;

typedef struct mappedKey
{
    mappedKey()
    {
        key = -1;
        messageType = -1;
        memset(messageName, 0, 32);
    }
    int key;
    int messageType;
    char messageName[32];
} mappedKey;

#endif