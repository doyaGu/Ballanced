/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//	     Load a texture using or not Threads
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTextureLoaderDecl();
CKERROR CreateTextureLoaderProto(CKBehaviorPrototype **);
int TextureLoader(const CKBehaviorContext &behcontext);
unsigned int TextureLoaderThreadFunction(void *arg);
CKERROR TextureLoaderCallback(const CKBehaviorContext &behcontext);

#ifdef WIN32
    #include <Windows.h>
    #define THREADED_LOADING
#endif

typedef struct LoadTextureThreadInfo
{
    XString TextureFile;
    int Slot;
    CKBOOL Loaded;
    CKTexture *Texture;
    CKBOOL Stop;
    CKContext *Context;
    CKBitmapProperties *bp;
    CKBitmapReader *breader;
    CKBOOL Error;

    LoadTextureThreadInfo()
    {
        Slot = 0;
        Loaded = 0;
        Texture = 0;
        Stop = 0;
        Context = 0;
        bp = 0;
        breader = 0;
        Error = 0;
    }

} LoadTextureThreadInfo;

CKBOOL LoadBufferToCKTexture(LoadTextureThreadInfo *LoadInfo, CKTexture *tex);

CKObjectDeclaration *FillTextureLoaderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Texture Load");
    od->SetDescription("Loads a texture file into a texture or a Sprite.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <SPAN CLASS=out>Failed: </SPAN>is activated when the process has failed.<BR>
    <BR>
    <SPAN CLASS=pin>File: </SPAN>name of the local file to read the texture from.<BR>
    <SPAN CLASS=pin>Slot: </SPAN>slot in which to load the texture.<BR>
    */
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xe85ab1, 0x312a731c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTextureLoaderProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    return od;
}

CKERROR CreateTextureLoaderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Texture Load");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");

    proto->DeclareOutput("Loaded");
    proto->DeclareOutput("Error");

    proto->DeclareInParameter("Texture File", CKPGUID_STRING);
    proto->DeclareInParameter("Slot", CKPGUID_INT, "0");

    proto->DeclareLocalParameter("Loading Texture Thread", CKPGUID_POINTER);
    proto->DeclareLocalParameter("Loading Info", CKPGUID_POINTER);

    proto->DeclareSetting("Use Thread", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTS));
    proto->SetFunction(TextureLoader);

    proto->SetBehaviorCallbackFct(TextureLoaderCallback, (CK_BEHAVIOR_CALLBACKMASK)CKCB_BEHAVIORRESET | CKCB_BEHAVIORSETTINGSEDITED | CKCB_BEHAVIORDETACH);

    *pproto = proto;
    return CK_OK;
}

int TextureLoader(const CKBehaviorContext &behcontext)
{
#ifdef THREADED_LOADING
    VxThread *VXT;
    LoadTextureThreadInfo *LoadInfo;
#endif
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    CKTexture *tex = (CKTexture *)beh->GetTarget();
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
    if (beh->IsInputActive(0))
    {
        // IN
        if (!UseThread)
        {
            // -----------------------------------------------------------------------------
            //  Do not use thread
            // -----------------------------------------------------------------------------

            beh->ActivateInput(0, FALSE);

            XString fname((CKSTRING)beh->GetInputParameterReadDataPtr(0));
            CKERROR res = CK_OK;
            if (!fname.Length())
            {
                res = CKERR_INVALIDPARAMETER;
            }
            else
            {
                ctx->GetPathManager()->ResolveFileName(fname, BITMAP_PATH_IDX);

                int slot = 0;
                beh->GetInputParameterValue(1, &slot);

                // Loading
                res = CKERR_INVALIDOBJECT;
                if (tex)
                    if (!tex->LoadImage(fname.Str(), slot))
                        res = CKERR_INVALIDFILE;
                    else
                        res = CK_OK;
            }
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
                VXT->SetName("TextureLoader");
                VXT->SetPriority(VXTP_NORMAL);
            }

            beh->GetLocalParameterValue(1, &LoadInfo);
            if (!LoadInfo)
            {
                LoadInfo = new LoadTextureThreadInfo;
            }

            // fill loadInfo structure
            LoadInfo->TextureFile = ((CKSTRING)beh->GetInputParameterReadDataPtr(0));

            ctx->GetPathManager()->ResolveFileName(LoadInfo->TextureFile, BITMAP_PATH_IDX);

            LoadInfo->Texture = tex;
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
                VXT->CreateThread(TextureLoaderThreadFunction, LoadInfo);
            }

            beh->SetLocalParameterValue(0, &VXT, sizeof(VxThread *));
            beh->SetLocalParameterValue(1, &LoadInfo, sizeof(LoadTextureThreadInfo *));

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
            if (LoadInfo->Loaded) // the texture is loaded
            {
                if (!LoadInfo->Error)
                {
                    LoadBufferToCKTexture(LoadInfo, LoadInfo->Texture);
                    beh->SetOutputParameterObject(0, LoadInfo->Texture);
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
unsigned int TextureLoaderThreadFunction(void *arg)
{
    LoadTextureThreadInfo *LoadInfo = (LoadTextureThreadInfo *)arg;

    while (!LoadInfo->Stop)
    {
        if (!LoadInfo->Loaded)
        {
            XString s = LoadInfo->TextureFile;
            int Width = 0, Height = 0;

            LoadInfo->Context->GetPathManager()->ResolveFileName(s, BITMAP_PATH_IDX, -1);

#define ExitLoad(error)               \
    {                                 \
        LoadInfo->breader->Release(); \
        LoadInfo->Error = TRUE;       \
        return 1;                     \
    }

            CKPathSplitter sp(LoadInfo->TextureFile.Str());
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
// Fill the CKTexture with the buffer
//
CKBOOL LoadBufferToCKTexture(LoadTextureThreadInfo *LoadInfo, CKTexture *tex)
{
    int oldslotcount = LoadInfo->Texture->GetSlotCount();
    if (LoadInfo->Slot >= oldslotcount)
    {
        LoadInfo->Texture->SetSlotCount(LoadInfo->Slot + 1);
    }

    //--- Create a new slot
    if (!LoadInfo->Texture->CreateImage(LoadInfo->bp->m_Format.Width, LoadInfo->bp->m_Format.Height, 32, LoadInfo->Slot))
    { // the slot cannot be created
        LoadInfo->Texture->SetSlotCount(oldslotcount);
        ExitLoad(FALSE);
    }

    CKBYTE *dst_ptr = LoadInfo->Texture->LockSurfacePtr(LoadInfo->Slot);
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
    LoadInfo->Texture->GetImageDesc(ImageDesc);
    LoadInfo->bp->m_Format.Image = (CKBYTE *)(LoadInfo->bp->m_Data);
    ImageDesc.Image = dst_ptr;
    VxDoBlit(LoadInfo->bp->m_Format, ImageDesc);

    // Source Image did not contain alpha information => force 	alpha to one
    if (!LoadInfo->bp->m_Format.AlphaMask)
        VxDoAlphaBlit(ImageDesc, 0xFF);

    LoadInfo->Texture->SetSlotFileName(LoadInfo->Slot, LoadInfo->TextureFile.Str());

    //--- Store the format that was used to load this bitmap
    LoadInfo->Texture->SetSaveFormat(LoadInfo->bp);

    LoadInfo->Texture->FreeVideoMemory();

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
CKERROR TextureLoaderCallback(const CKBehaviorContext &behcontext)
{
#ifdef THREADED_LOADING
    VxThread *VXT;
    LoadTextureThreadInfo *LoadInfo;
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