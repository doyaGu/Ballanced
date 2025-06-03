/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	     Load a sprite using or not Threads
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSpriteLoaderDecl();
CKERROR CreateSpriteLoaderProto(CKBehaviorPrototype **);
int SpriteLoader(const CKBehaviorContext &behcontext);
unsigned int SpriteLoaderThreadFunction(void *arg);
CKERROR SpriteLoaderCallback(const CKBehaviorContext &behcontext);

#ifdef WIN32
    #include <Windows.h>
    #define THREADED_LOADING
#endif

typedef struct LoadspriteThreadInfo
{
    XString spriteFile;
    int Slot;
    CKBOOL Loaded;
    CKSprite *sprite;
    CKBOOL Stop;
    CKContext *Context;
    CKBitmapProperties *bp;
    CKBitmapReader *breader;
    CKBOOL Error;

    LoadspriteThreadInfo()
    {
        Slot = 0;
        Loaded = 0;
        sprite = 0;
        Stop = 0;
        Context = 0;
        bp = 0;
        breader = 0;
        Error = 0;
    }

} LoadspriteThreadInfo;

CKBOOL LoadBufferToCKSprite(LoadspriteThreadInfo *LoadInfo, CKSprite *tex);

CKObjectDeclaration *FillSpriteLoaderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("sprite Load");
    od->SetDescription("Loads a sprite file into a sprite or a ABPosFinder.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Failed: </SPAN>is activated when the process has failed.<BR>
    <BR>
    <SPAN CLASS=pin>File: </SPAN>name of the local file to read the sprite from.<BR>
    <SPAN CLASS=pin>Slot: </SPAN>slot in which to load the sprite.<BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xe85ae1, 0x317a771c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSpriteLoaderProto);
    od->SetCompatibleClassId(CKCID_SPRITE);
    return od;
}

CKERROR CreateSpriteLoaderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("sprite Load");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Loaded");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("sprite File", CKPGUID_STRING);
    proto->DeclareInParameter("Slot", CKPGUID_INT, "0");

    proto->DeclareLocalParameter("Loading sprite Thread", CKPGUID_POINTER);
    proto->DeclareLocalParameter("Loading Info", CKPGUID_POINTER);

    proto->DeclareSetting("Use Thread", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTS));
    proto->SetFunction(SpriteLoader);

    proto->SetBehaviorCallbackFct(SpriteLoaderCallback, (CK_BEHAVIOR_CALLBACKMASK)CKCB_BEHAVIORRESET | CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORDETACH);

    *pproto = proto;
    return CK_OK;
}

int SpriteLoader(const CKBehaviorContext &behcontext)
{
#ifdef THREADED_LOADING
    VxThread *VXT;
    LoadspriteThreadInfo *LoadInfo;
#endif
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKSprite *tex = (CKSprite *)beh->GetTarget();
    if (!tex)
    {
        beh->ActivateOutput(1);
        return CKBR_PARAMETERERROR;
    }

    CKBOOL UseThread = FALSE;
#ifdef THREADED_LOADING
    if (beh->GetLocalParameter(2))
        beh->GetLocalParameterValue(2, &UseThread);
#endif
    if (beh->IsInputActive(0)) // IN
    {
        if (!UseThread)
        {
            // -----------------------------------------------------------------------------
            //  Do not use thread
            // -----------------------------------------------------------------------------

            beh->ActivateInput(0, FALSE);

            XString fname((CKSTRING)beh->GetInputParameterReadDataPtr(0));
            ctx->GetPathManager()->ResolveFileName(fname, BITMAP_PATH_IDX);

            int slot = 0;
            beh->GetInputParameterValue(1, &slot);

            // Loading
            CKERROR res = CKERR_INVALIDOBJECT;
            if (tex)
                if (!tex->LoadImage(fname.Str(), slot))
                    res = CKERR_INVALIDFILE;
                else
                    res = CK_OK;

            if (res != CK_OK)
                beh->ActivateOutput(1);
            else
            {
                beh->SetOutputParameterObject(0, tex);
                beh->ActivateOutput(0);
            }

            return CKBR_OK;
        }
#ifdef THREADED_LOADING
        else
        {
            // -----------------------------------------------------------------------------
            //  Use Thread
            // -----------------------------------------------------------------------------
            beh->ActivateInput(0, FALSE);

            beh->GetLocalParameterValue(0, &VXT);
            if (!VXT) // Start a Thread if needed
            {
                VXT = new VxThread();
                VXT->SetName("spriteLoader");
                VXT->SetPriority(VXTP_NORMAL);
            }

            beh->GetLocalParameterValue(1, &LoadInfo);
            if (!LoadInfo)
            {
                LoadInfo = new LoadspriteThreadInfo;
            }

            // fill loadInfo structure
            LoadInfo->spriteFile = ((CKSTRING)beh->GetInputParameterReadDataPtr(0));

            ctx->GetPathManager()->ResolveFileName(LoadInfo->spriteFile, BITMAP_PATH_IDX);

            LoadInfo->sprite = tex;
            LoadInfo->Context = behcontext.Context;
            LoadInfo->bp = NULL;
            LoadInfo->breader = NULL;
            LoadInfo->Error = FALSE;

            int slot = 0;
            beh->GetInputParameterValue(1, &slot);
            LoadInfo->Slot = slot;
            LoadInfo->Loaded = FALSE;
            LoadInfo->Stop = FALSE;

            if (!VXT->IsCreated())
            {
                VXT->CreateThread(SpriteLoaderThreadFunction, LoadInfo);
            }

            beh->SetLocalParameterValue(0, &VXT, sizeof(VxThread *));
            beh->SetLocalParameterValue(1, &LoadInfo, sizeof(LoadspriteThreadInfo *));

            return CKBR_ACTIVATENEXTFRAME;
        }
#endif
    }

    if (beh->IsInputActive(1)) // Kill thread
    {
#ifdef THREADED_LOADING
        beh->GetLocalParameterValue(0, &VXT);
        beh->GetLocalParameterValue(1, &LoadInfo);

        LoadInfo->Stop = TRUE;
        VXT->Wait();
        delete VXT;
        VXT = NULL;
        beh->SetLocalParameterValue(0, &VXT);
        delete LoadInfo;
        LoadInfo = NULL;
        beh->SetLocalParameterValue(1, &LoadInfo);
#endif
        return CKBR_OK;
    }
    // no activation, check if finished (for thread)
    else
    {
        if (UseThread)
        {
#ifdef THREADED_LOADING
            beh->GetLocalParameterValue(0, &VXT);
            beh->GetLocalParameterValue(1, &LoadInfo);
            if (LoadInfo->Loaded) // the sprite is loaded
            {
                if (!LoadInfo->Error)
                {
                    LoadBufferToCKSprite(LoadInfo, LoadInfo->sprite);
                    beh->SetOutputParameterObject(0, LoadInfo->sprite);
                    beh->ActivateOutput(0, TRUE);
                    return CKBR_OK;
                }
                else
                {
                    beh->ActivateOutput(1, TRUE);
                    return CKBR_OK;
                }
            }
#endif
        } // Thread Loading
    }
    return CKBR_ACTIVATENEXTFRAME;
}

//
// Thread Function
//
#ifdef THREADED_LOADING
unsigned int SpriteLoaderThreadFunction(void *arg)
{
    LoadspriteThreadInfo *LoadInfo = (LoadspriteThreadInfo *)arg;

    while (!LoadInfo->Stop)
    {
        if (!LoadInfo->Loaded)
        {
            XString s = LoadInfo->spriteFile;
            int Width = 0, Height = 0;

            LoadInfo->Context->GetPathManager()->ResolveFileName(s, BITMAP_PATH_IDX, -1);

#define ExitLoad(error)               \
    {                                 \
        LoadInfo->breader->Release(); \
        LoadInfo->Error = TRUE;       \
        return 1;                     \
    }

            CKPathSplitter sp(LoadInfo->spriteFile.Str());
            CKFileExtension CKext(&sp.GetExtension()[1]);

            LoadInfo->breader = (CKBitmapReader *)CKGetPluginManager()->GetBitmapReader(CKext);
            if (!LoadInfo->breader)
            {
                LoadInfo->Error = TRUE;
                return 1;
            }

            //--- Load the bitmap file in the buffer
            if (LoadInfo->breader->ReadFile(s.Str(), &LoadInfo->bp) || !LoadInfo->bp || !LoadInfo->bp->m_Data)
            {
                LoadInfo->breader->Release();
                LoadInfo->Error = TRUE;
            }
            else
            {
                LoadInfo->Error = FALSE;
            }

            LoadInfo->Loaded = TRUE;
        }
        else
            Sleep(100);
    }
    return 0x00004321;
}

//
// Fill the CKSprite with the buffer
//
CKBOOL LoadBufferToCKSprite(LoadspriteThreadInfo *LoadInfo, CKSprite *tex)
{
    int oldslotcount = LoadInfo->sprite->GetSlotCount();
    if (LoadInfo->Slot >= oldslotcount)
    {
        LoadInfo->sprite->SetSlotCount(LoadInfo->Slot + 1);
    }

    //--- Create a new slot
    if (!LoadInfo->sprite->Create(LoadInfo->bp->m_Format.Width, LoadInfo->bp->m_Format.Height, 32, LoadInfo->Slot))
    { // the slot cannot be created
        LoadInfo->sprite->SetSlotCount(oldslotcount);
        ExitLoad(FALSE);
    }

    CKBYTE *dst_ptr = LoadInfo->sprite->LockSurfacePtr(LoadInfo->Slot);
    if (!dst_ptr)
    {
        LoadInfo->breader->ReleaseMemory(LoadInfo->bp->m_Data);
        LoadInfo->bp->m_Data = NULL;
        delete LoadInfo->bp->m_Format.ColorMap;
        LoadInfo->bp->m_Format.ColorMap = NULL;
        ExitLoad(FALSE);
    }

    //--- Blit Reader image to the newly created slot
    VxImageDescEx ImageDesc;
    LoadInfo->sprite->GetSystemTextureDesc(ImageDesc);
    LoadInfo->bp->m_Format.Image = (CKBYTE *)(LoadInfo->bp->m_Data);
    ImageDesc.Image = dst_ptr;
    VxDoBlit(LoadInfo->bp->m_Format, ImageDesc);

    // Source Image did not contain alpha information => force 	alpha to one
    if (!LoadInfo->bp->m_Format.AlphaMask)
        VxDoAlphaBlit(ImageDesc, 0xFF);

    LoadInfo->sprite->SetSlotFileName(LoadInfo->Slot, LoadInfo->spriteFile.Str());

    //--- Store the format that was used to load this bitmap
    LoadInfo->sprite->SetSaveFormat(LoadInfo->bp);

    LoadInfo->sprite->FreeVideoMemory();

    LoadInfo->breader->ReleaseMemory(LoadInfo->bp->m_Data);
    LoadInfo->bp->m_Data = NULL;
    delete LoadInfo->bp->m_Format.ColorMap;
    LoadInfo->bp->m_Format.ColorMap = NULL;
    return TRUE;
}
#endif

//
//  CallBack
//
CKERROR SpriteLoaderCallback(const CKBehaviorContext &behcontext)
{
#ifdef THREADED_LOADING
    VxThread *VXT;
    LoadspriteThreadInfo *LoadInfo;
#endif
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORRESET:
    case CKM_BEHAVIORDETACH:
    {
#ifdef THREADED_LOADING
        if (!beh->GetLocalParameter(0) || !beh->GetLocalParameter(1))
            break;
        beh->GetLocalParameterValue(0, &VXT);
        beh->GetLocalParameterValue(1, &LoadInfo);
        if (VXT)
        {
            if (LoadInfo)
            {
                LoadInfo->Stop = TRUE;
            }
            VXT->Wait();
            delete VXT;
            VXT = NULL;
            beh->SetLocalParameterValue(0, &VXT);
        }
        if (LoadInfo)
        {
            delete LoadInfo;
            LoadInfo = NULL;
            beh->SetLocalParameterValue(1, &LoadInfo);
        }
#endif
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
#ifdef THREADED_LOADING
        if (!beh->GetLocalParameter(2))
            break;

        CKBOOL UseThread = FALSE;
        beh->GetLocalParameterValue(2, &UseThread);
        if (UseThread)
        {
            if (beh->GetInputCount() == 1)
                beh->CreateInput("Kill Thread");
        }
        else
        {
            if (beh->GetInputCount() == 2)
                beh->RemoveInput(1);
        }
#endif
    }
    break;
    }
    return CKBR_OK;
}