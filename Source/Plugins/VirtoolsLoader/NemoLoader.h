#ifndef PLUGINS_NEMOLOADER_H
#define PLUGINS_NEMOLOADER_H

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "CKAll.h"

#define VIRTOOLS_COMPOSITION_READER_VERSION 0x0000001
#define VIRTOOLS_COMPOSITION_READER_GUID CKGUID(0x6B013E56, 0x64BA597E)

#define VIRTOOLS_OBJECT_READER_VERSION 0x0000001
#define VIRTOOLS_OBJECT_READER_GUID CKGUID(0x6117625E, 0x247C3CF2)

#define VIRTOOLS_BEHAVIORS_READER_VERSION 0x0000001
#define VIRTOOLS_BEHAVIORS_READER_GUID CKGUID(0x54CB32FD, 0x17E8715D)

#define VIRTOOLS_PLAYER_READER_VERSION 0x0000001
#define VIRTOOLS_PLAYER_READER_GUID CKGUID(0x28371AAB, 0x6F1A4498)

class CKNemoLoader : public CKModelReader
{
public:
    CKNemoLoader() {}
    ~CKNemoLoader() {}

    void Release() { delete this; };

    virtual CKPluginInfo *GetReaderInfo();

    virtual int GetOptionsCount() { return 0; }

    virtual CKSTRING GetOptionDescription(int i) { return NULL; }

    virtual CK_DATAREADER_FLAGS GetFlags() { return (CK_DATAREADER_FLAGS)(CK_DATAREADER_FILELOAD | CK_DATAREADER_FILESAVE); }

    virtual CKERROR Load(CKContext *context, CKSTRING FileName, CKObjectArray *liste, CKDWORD LoadFlags, CKCharacter *carac = NULL);

    virtual CKERROR Save(CKContext *context, CKSTRING FileName, CKObjectArray *liste, CKDWORD SaveFlags);
};

#endif // PLUGINS_NEMOLOADER_H