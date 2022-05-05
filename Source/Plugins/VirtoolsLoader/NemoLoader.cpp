#include "NemoLoader.h"

#include <stdio.h>
#include <string.h>

#ifdef CK_LIB
#define RegisterBehaviorDeclarations Register_NemoLoader_BehaviorDeclarations
#define InitInstance _NemoLoader_InitInstance
#define ExitInstance _NemoLoader_ExitInstance
#define CKGetPluginInfoCount CKGet_NemoLoader_PluginInfoCount
#define CKGetPluginInfo CKGet_NemoLoader_PluginInfo
#define g_PluginInfo g_NemoLoader_PluginInfo
#define CKGetReader CKGet_NemoLoader_Reader
#else
#define RegisterBehaviorDeclarations RegisterBehaviorDeclarations
#define InitInstance InitInstance
#define ExitInstance ExitInstance
#define CKGetPluginInfoCount CKGetPluginInfoCount
#define CKGetPluginInfo CKGetPluginInfo
#define g_PluginInfo g_PluginInfo
#define CKGetReader CKGetReader
#endif

#define READER_COUNT 4

/*************************************************************************
 PLUGIN DECLARATION
**************************************************************************/
CKPluginInfo g_PluginInfo[READER_COUNT] = {
    CKPluginInfo(VIRTOOLS_COMPOSITION_READER_GUID,           // GUID
                 "Cmo",                                      // Extension supported
                 "Virtools Composition",                     // Reader Name
                 "Virtools",                                 // Company
                 "Virtools Plugin (Compositions)",           // Summary String
                 VIRTOOLS_COMPOSITION_READER_VERSION,        // Reader Version
                 NULL,                                       // No Init Instance function needed
                 NULL,                                       // No Exit Instance function needed
                 CKPLUGIN_MODEL_READER),                     // Plugin Type : Model Reader
    CKPluginInfo(VIRTOOLS_OBJECT_READER_GUID,                // GUID
                 "Nmo",                                      // Extension supported
                 "Virtools Object",                          // Reader Name
                 "Virtools",                                 // Company
                 "Virtools Plugin (Object)",                 // Summary String
                 VIRTOOLS_OBJECT_READER_VERSION,             // Reader Version
                 NULL,                                       // No Init Instance function needed
                 NULL,                                       // No Exit Instance function needed
                 CKPLUGIN_MODEL_READER),                     // Plugin Type : Model Reader
    CKPluginInfo(VIRTOOLS_BEHAVIORS_READER_GUID,             // GUID
                 "Nms",                                      // Extension supported
                 "Virtools Behaviors Graph/Script",          // Reader Name
                 "Virtools",                                 // Company
                 "Virtools Plugin (Behaviors Graph/Script)", // Summary String
                 VIRTOOLS_BEHAVIORS_READER_VERSION,          // Reader Version
                 NULL,                                       // No Init Instance function needed
                 NULL,                                       // No Exit Instance function needed
                 CKPLUGIN_MODEL_READER),                     // Plugin Type : Model Reader
    CKPluginInfo(VIRTOOLS_PLAYER_READER_GUID,                // GUID
                 "Vmo",                                      // Extension supported
                 "Virtools Player",                          // Reader Name
                 "Virtools",                                 // Company
                 "Virtools Plugin (Player)",                 // Summary String
                 VIRTOOLS_PLAYER_READER_VERSION,             // Reader Version
                 NULL,                                       // No Init Instance function needed
                 NULL,                                       // No Exit Instance function needed
                 CKPLUGIN_MODEL_READER),                     // Plugin Type : Model Reader
};

/**********************************************
 Called by the engine when a file with the CMO,
 NMO, NMS, and VMO extension is being loaded,
 a reader has to be created.
***********************************************/
CKDataReader *CKGetReader(int pos)
{
    return new CKNemoLoader();
}

/******************************************
 Returns number of the reader which is the
 same as given to the engine at
 initialisation.
*******************************************/
int CKGetPluginInfoCount()
{
    return READER_COUNT;
}

/******************************************
 Returns information about the reader which
 is the same as given to the engine at
 initialisation.
*******************************************/
CKPluginInfo *CKGetPluginInfo(int index)
{
    return &g_PluginInfo[index];
}

/*************************************************************************
 PLUGIN IMPLEMENTATION
**************************************************************************/

CKPluginInfo *CKNemoLoader::GetReaderInfo()
{
    return (CKPluginInfo *)g_PluginInfo;
}

CKERROR CheckFileType(CKSTRING FileName)
{
    FILE *fp = fopen(FileName, "rt");
    if (!fp)
    {
        return CKERR_INVALIDFILE;
    }

    char buffer[8];
    fread(buffer, sizeof(char), 4, fp);
    if (strncmp(buffer, "Nemo", 4) != 0)
    {
        return CKERR_INVALIDFILE;
    }
    fclose(fp);

    return CK_OK;
}

CKERROR CKNemoLoader::Load(CKContext *context, CKSTRING FileName, CKObjectArray *liste, CKDWORD LoadFlags, CKCharacter *carac)
{
    if (!liste)
    {
        return CKERR_INVALIDPARAMETER;
    }

    CKERROR ret = CheckFileType(FileName);
    if (ret != CK_OK)
    {
        return ret;
    }

    CKFile *file = context->CreateCKFile();
    if (!file)
    {
        return CKERR_OUTOFMEMORY;
    }

    ret = file->Load(FileName, liste, (CK_LOAD_FLAGS)LoadFlags);
    if (ret != CK_OK)
    {
        context->DeleteCKFile(file);
        return ret;
    }

    file->UpdateAndApplyAnimationsTo(carac);
    context->DeleteCKFile(file);
    return CK_OK;
}

CKERROR CKNemoLoader::Save(CKContext *context, CKSTRING FileName, CKObjectArray *liste, CKDWORD SaveFlags)
{
    if (!liste)
    {
        return CKERR_INVALIDPARAMETER;
    }

    CKFile *file = context->CreateCKFile();
    if (!file)
    {
        return CKERR_OUTOFMEMORY;
    }

    file->StartSave(FileName);
    file->SaveObjects(liste);
    CKERROR ret = file->EndSave();

    context->DeleteCKFile(file);

    return ret;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls((HMODULE)hModule);
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    
    return TRUE;
}
