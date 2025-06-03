/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//              StaticLightMap
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"
#include "TextureProcessing.h"
#include "assert.h"
#include "LightMapGenerator.h"

CKObjectDeclaration *FillBehaviorStaticLightMapDecl();
CKERROR CreateStaticLightMapProto(CKBehaviorPrototype **);
int StaticLightMap(const CKBehaviorContext &behcontext);
CKERROR StaticLightMapCallBackObject(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorStaticLightMapDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Static Lightmap");
    od->SetDescription("Generates objects with lightmaps (textures containing lighting and shadows).");
    /* rem:
     */
    od->SetCategory("Lights/FX");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4a797d21, 0x214d37fa));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateStaticLightMapProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateStaticLightMapProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Static Lightmap");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Receiving Objects", CKPGUID_GROUP);
    proto->DeclareInParameter("Occluder Objects", CKPGUID_GROUP);
    proto->DeclareInParameter("Lights", CKPGUID_GROUP);
    proto->DeclareInParameter("LightMap Resolution", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("Supersampling", CKPGUID_INT, "1");
    proto->DeclareInParameter("Blur Pass", CKPGUID_INT, "0");
    proto->DeclareInParameter("Threshold", CKPGUID_ANGLE, "0:10");

    // The face / texture array
    proto->DeclareLocalParameter(NULL, CKPGUID_POINTER);
    proto->DeclareSetting("Display Computation Time", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(StaticLightMap);
    proto->SetBehaviorCallbackFct(StaticLightMapCallBackObject);

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
    THRESHOLD
};

enum
{
    DATA,
    SHOWTIME
};

/*
CKBOOL VisibleBSP(CKContext* ctx,CK3dEntity* caster,const VxVector& start,const VxVector& dest)
{
}
*/

void StaticLightMapsCreation(CKBehavior *beh)
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

    LightmapGenerator *lg = NULL;
    beh->GetLocalParameterValue(0, &lg);

    if (lg)
    {
        lg->SetReceivers((CKGroup *)beh->GetInputParameterObject(RECEIVERS)); // Receivers
        lg->SetOccluders((CKGroup *)beh->GetInputParameterObject(OCCLUDERS)); // Occluders
        lg->SetLights((CKGroup *)beh->GetInputParameterObject(LIGHTS));       // Lights

        lg->Generate(units, threshold, supersample, blur);

        // We now create the objects to support lightmap
        lg->GenerateObjects(beh->GetCKContext());

        lg->Clean(FALSE);
    }

    float t1 = ctx->GetTimeManager()->GetTime();

    CKBOOL displaytime = TRUE;
    beh->GetLocalParameterValue(SHOWTIME, &displaytime);
    if (displaytime)
        ctx->OutputToConsoleEx("Time : %f", (t1 - t0));
}

int StaticLightMap(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // IO Activation
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    StaticLightMapsCreation(beh);

    return CKBR_OK;
}

CKERROR StaticLightMapCallBackObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIORCREATE:
    case CKM_BEHAVIORLOAD:
    {
        // First we get the lightmap generator
        LightmapGenerator *lg = new LightmapGenerator(behcontext.Context);
        beh->SetLocalParameterValue(0, &lg);
    }
    break;

    case CKM_BEHAVIORDETACH:
    {
        // TODO(Normalement sur le delete il faudrait remove la render callback (probleme souleve par francis : trouver un moyen plus global)

        // First we get the lightmap generator
        LightmapGenerator *lg;
        beh->GetLocalParameterValue(0, &lg);

        delete lg;
        lg = NULL;

        beh->SetLocalParameterValue(0, &lg);
    }
    break;
    }
    return CKBR_OK;
}
