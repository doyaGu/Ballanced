/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              LightMap
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

#include "CKAll.h"
#include "TextureProcessing.h"
#include "assert.h"
#include "LightmapGenerator.h"

#ifdef WIN32
#include <windows.h>
#endif

CKObjectDeclaration *FillBehaviorLightMapDecl();
CKERROR CreateLightMapProto(CKBehaviorPrototype **);
int LightMap(const CKBehaviorContext &behcontext);
CKERROR LightMapCallBackObject(const CKBehaviorContext &behcontext);

#ifdef USEMT
#define MAXTHREADS 2

struct LMThreadParams
{
    LightmapGenerator *mLG;
    float Density;
    float Threshold;
    int SuperSampling;
    int Blur;

    HANDLE beginEvent;
    HANDLE doneEvent;
    // need padding?  probably not, might need it in LightmapGenerator if allocated
    // consecutively
};

struct ThreadedLightmapGenerator
{
    LightmapGenerator lg1;
    LightmapGenerator lg2;
    ThreadedLightmapGenerator(CKContext *ctx);

    void ShowPolygons(CKBOOL s)
    {
        lg1.ShowPolygons(s);
        lg2.ShowPolygons(s);
    }
    void SetOpacity(const CKDWORD opacity)
    {
        lg1.SetOpacity(opacity);
        lg2.SetOpacity(opacity);
    }

    void AttachRenderCallbacks()
    {
        lg1.AttachRenderCallbacks();
        lg2.AttachRenderCallbacks();
    }
    void RemoveRenderCallbacks()
    {
        lg1.RemoveRenderCallbacks();
        lg2.RemoveRenderCallbacks();
    }
    void MarkTextureAsPrivate(CKBOOL priv)
    {
        lg1.MarkTextureAsPrivate(priv);
        lg2.MarkTextureAsPrivate(priv);
    }

    void Clean(CKBOOL iDestroyTextures)
    {
        lg1.Clean(iDestroyTextures);
        lg2.Clean(iDestroyTextures);
    }
    void Save(CKStateChunk *chunk) {}
    void Load(CKStateChunk *chunk) {}

    // world information functiosn : call these before Generate !
    void SetReceivers(CKGroup *g)
    {
        int Count = g ? g->GetObjectCount() : m_Context->GetObjectsCountByClassID(CKCID_3DOBJECT);
        int Mid = Count / 2;
        lg1.SetReceivers(g, 0, Mid);
        lg2.SetReceivers(g, Mid, Count - Mid);
    }
    void SetOccluders(CKGroup *g)
    {
        lg1.SetOccluders(g);
        lg2.SetOccluders(g);
    }
    void SetLights(CKGroup *g)
    {
        lg1.SetLights(g);
        lg2.SetLights(g);
    }

    // Lightmap Generation function : does all the job in here !
    void Generate(float Density, float Threshold, int SuperSampling, int Blur);

    // Lightmapped Object creation
    void GenerateObjects(CKContext *iCtx)
    {
    }

    CKContext *m_Context;

    // Thread stuff
    static DWORD WINAPI LMG_WorkerThreadFunc(PVOID param);

    // Reuse main thread, so only launch MAXTHREADS-1
    HANDLE hThreads[MAXTHREADS - 1];
    HANDLE beginEvents[MAXTHREADS - 1];
    HANDLE doneEvents[MAXTHREADS - 1];
    LMThreadParams thrParams[MAXTHREADS - 1];
};

ThreadedLightmapGenerator::ThreadedLightmapGenerator(CKContext *ctx)
    : lg1(ctx), lg2(ctx), m_Context(ctx)
{

    assert(MAXTHREADS == 2); // right now we only want 2 threads
    beginEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
    doneEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL);

    thrParams[0].mLG = &lg1; // We give it lg1, because in the case of one receiver than
                             // lg2 does the work and lg1 does nothing.  Quicker this way
    thrParams[0].beginEvent = beginEvents[0];
    thrParams[0].doneEvent = doneEvents[0];
    hThreads[0] = CreateThread(NULL, NULL, LMG_WorkerThreadFunc, &thrParams[0],
                               0, NULL); // w2k/xp specific
}

DWORD WINAPI ThreadedLightmapGenerator::LMG_WorkerThreadFunc(PVOID param)
{
    LMThreadParams *lParam = (LMThreadParams *)param;
    // Copy handles

    HANDLE myBeginEvent = lParam->beginEvent;
    HANDLE myDoneEvent = lParam->doneEvent;
    LightmapGenerator *myLG = lParam->mLG;

    for (;;)
    {
        WaitForSingleObject(myBeginEvent, INFINITE);

        // Copy particular params
        float Density = lParam->Density;
        float Threshold = lParam->Threshold;
        int SuperSampling = lParam->SuperSampling;
        int Blur = lParam->Blur;

        myLG->Generate(Density, Threshold, SuperSampling, Blur);

        SetEvent(myDoneEvent);
    }
}
void ThreadedLightmapGenerator::Generate(float Density, float Threshold,
                                         int SuperSampling, int Blur)
{
    // copy parameters into thrParams
    thrParams[0].Density = Density;
    thrParams[0].Threshold = Threshold;
    thrParams[0].SuperSampling = SuperSampling;
    thrParams[0].Blur = Blur;

    // Pre-serial
    lg1.Clean();
    lg2.Clean();
    // Signal other thread to start
    for (int i = 0; i < MAXTHREADS - 1; i++)
        SetEvent(beginEvents[i]);

    // Main thread reuse
    lg2.Generate(Density, Threshold, SuperSampling, Blur);

    // Now wait for other thread to finish
    WaitForMultipleObjects(MAXTHREADS - 1, doneEvents, TRUE, INFINITE);

    // Need to be serial
    lg1.PackAllTextures();
    lg2.PackAllTextures();
}

#define GetLG                             \
    ThreadedLightmapGenerator *lg = NULL; \
    beh->GetLocalParameterValue(0, &lg);

#else

#define GetLG                     \
    LightmapGenerator *lg = NULL; \
    beh->GetLocalParameterValue(0, &lg);

#endif

CKObjectDeclaration *FillBehaviorLightMapDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Lightmap");
    od->SetDescription("Calculates lightmaps (textures containing lighting and shadows) for a set of objects with a set of lights.");
    /* rem:
     */
    od->SetCategory("Lights/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4a797d21, 0x214d37f9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLightMapProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateLightMapProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Lightmap");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");
    proto->DeclareInput("Generate");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");
    proto->DeclareOutput("Exit Generate");

    proto->DeclareInParameter("Receiving Objects", CKPGUID_GROUP);
    proto->DeclareInParameter("Occluder Objects", CKPGUID_GROUP);
    proto->DeclareInParameter("Lights", CKPGUID_GROUP);
    proto->DeclareInParameter("LightMap Resolution", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Supersampling", CKPGUID_INT, "1");
    proto->DeclareInParameter("Blur Pass", CKPGUID_INT, "0");
    proto->DeclareInParameter("Threshold", CKPGUID_ANGLE, "0:10");
    proto->DeclareInParameter("Opacity", CKPGUID_COLOR, "255,255,255,255");

    // The face / texture array
    proto->DeclareLocalParameter(NULL, CKPGUID_POINTER);
    proto->DeclareLocalParameter(NULL, CKPGUID_STATECHUNK);
    proto->DeclareSetting("Save", CKPGUID_BOOL, "TRUE");
    proto->DeclareSetting("Show Polygons", CKPGUID_BOOL, "FALSE");
    proto->DeclareSetting("Display Computation Time", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(LightMap);
    proto->SetBehaviorCallbackFct(LightMapCallBackObject);

    *pproto = proto;
    return CK_OK;
}

enum
{
    RECEIVERS,
    OCCLUDERS,
    LIGHTS,
    DENSITY,
    SUPERSAMPLING,
    BLUR,
    THRESHOLD,
    OPACITY
};

enum
{
    DATA,
    STATECHUNK,
    SAVE,
    SHOWPOLY,
    SHOWTIME
};
/*
CKBOOL VisibleBSP(CKContext* ctx,CK3dEntity* caster,const VxVector& start,const VxVector& dest)
{
}
*/

void LightMapsCreation(CKBehavior *beh)
{

    CKContext *ctx = beh->GetCKContext();

    float t0 = ctx->GetTimeManager()->GetTime();
    ///
    // Reading Parameters

    float units = 4.0f;
    beh->GetInputParameterValue(DENSITY, &units);

    int supersample = 1;
    beh->GetInputParameterValue(SUPERSAMPLING, &supersample);
    if (supersample < 1)
        supersample = 1;

    int blur = 0;
    beh->GetInputParameterValue(BLUR, &blur);

    float threshold = 0.5f;
    beh->GetInputParameterValue(THRESHOLD, &threshold);
    threshold = cosf(threshold);

    GetLG
        //	LightmapGenerator* lg = NULL;
        //	beh->GetLocalParameterValue(0,&lg);

        if (lg)
    {
        lg->SetReceivers((CKGroup *)beh->GetInputParameterObject(RECEIVERS)); // Receivers
        lg->SetOccluders((CKGroup *)beh->GetInputParameterObject(OCCLUDERS)); // Occluders
        lg->SetLights((CKGroup *)beh->GetInputParameterObject(LIGHTS));       // Lights

        lg->Generate(units, threshold, supersample, blur);
    }

    float t1 = ctx->GetTimeManager()->GetTime();

    CKBOOL displaytime = TRUE;
    beh->GetLocalParameterValue(SHOWTIME, &displaytime);
    if (displaytime)
        ctx->OutputToConsoleEx("Time : %f", (t1 - t0));
}

int LightMap(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    if (beh->IsInputActive(2))
    {
        // we enter by 'Generate'
        beh->ActivateInput(2, FALSE);
        beh->ActivateOutput(2);

        LightMapsCreation(beh);
    }

    if (beh->IsInputActive(1))
    {
        // we enter by 'OFF'
        beh->ActivateInput(1, FALSE);
        beh->ActivateOutput(1);

        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        // we enter by 'ON'
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0);
    }

    GetLG
    //	LightmapGenerator* lg;
    //	beh->GetLocalParameterValue(0,&lg);

    CKBOOL showpoly = FALSE;
    beh->GetLocalParameterValue(3, &showpoly);
    lg->ShowPolygons(showpoly);

    VxColor opacity(1.0f, 1.0f, 1.0f, 1.0f);
    beh->GetInputParameterValue(OPACITY, &opacity);
    lg->SetOpacity(RGBAFTOCOLOR(&opacity));

    lg->AttachRenderCallbacks();

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR LightMapCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    {
        // First we get the lightmap generator
#ifdef USEMT
        // First we get the lightmap generator
        ThreadedLightmapGenerator *lg = new ThreadedLightmapGenerator(behcontext.Context);
        beh->SetLocalParameterValue(0, &lg);

#else
        // First we get the lightmap generator
        LightmapGenerator *lg = new LightmapGenerator(behcontext.Context);
        beh->SetLocalParameterValue(0, &lg);
#endif;
    }
    break;
    case CKM_BEHAVIORSETTINGSEDITED:
    {
        // LightMapsCreation(beh);
    }
    break;
    case CKM_BEHAVIORLOAD:
    {
#ifdef USEMT
        // First we get the lightmap generator
        ThreadedLightmapGenerator *lg = new ThreadedLightmapGenerator(behcontext.Context);
        beh->SetLocalParameterValue(0, &lg);

#else
        // First we get the lightmap generator
        LightmapGenerator *lg = new LightmapGenerator(behcontext.Context);
        beh->SetLocalParameterValue(0, &lg);
#endif

        // we get the chunk stocked in local parameter
        CKStateChunk *chunk;
        beh->GetLocalParameterValue(1, &chunk);

        // we try to load it
        lg->Load(chunk);

        // we clear the chunk
        chunk->Clear();
    }
    break;

    case CKM_BEHAVIORDETACH:
    {
        // First we get the lightmap generator
        GetLG

        // removing the render callback
        if (lg)
        {
            lg->RemoveRenderCallbacks();
        }
    }
    break;
    case CKM_BEHAVIORDELETE:
    {
        // First we get the lightmap generator
        GetLG

        // removing the render callback
        if (lg)
        {
            delete lg;
            lg = NULL;
        }

        beh->SetLocalParameterValue(0, &lg);
    }
    break;
    case CKM_BEHAVIORPRESAVE:
    {
        // First we get the lightmap generator
        GetLG
        // LightmapGenerator* lg;
        // beh->GetLocalParameterValue(0,&lg);

        CKBOOL save = TRUE;
        beh->GetLocalParameterValue(2, &save);

        // the user does not want its lightmap saved
        if (!save)
        {
            // we have to mark the texture as private
            lg->MarkTextureAsPrivate(TRUE);
        }
        else
        {
            // we get the chunk stocked in local parameter
            CKStateChunk *chunk;
            beh->GetLocalParameterValue(1, &chunk);

            // we save the lightmaps
            lg->Save(chunk);
        }
    }
    break;
    case CKM_BEHAVIORPOSTSAVE:
    {
        // First we get the lightmap generator
        GetLG
        // LightmapGenerator* lg;
        // beh->GetLocalParameterValue(0,&lg);

        // we have to mark the texture as private
        lg->MarkTextureAsPrivate(FALSE);

        // we get the chunk stocked in local parameter
        CKStateChunk *chunk;
        beh->GetLocalParameterValue(1, &chunk);

        // all we have to do now is clear the chunk to gain memory
        chunk->Clear();
    }
    break;
    }
    return CKBR_OK;
}
