//////////////////////////////////////
//////////////////////////////////////
//
//        TT SinusMeshdeform
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTSinusMeshdeformBehaviorProto(CKBehaviorPrototype **pproto);
int TTSinusMeshdeform(const CKBehaviorContext& behcontext);
CKERROR TTSinusMeshdeformCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTSinusMeshdeformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT SinusMeshdeform");
    od->SetDescription("generiert Sinuswellen");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f907f12,0x16af15d6));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTSinusMeshdeformBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTSinusMeshdeformBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT SinusMeshdeform");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit On");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("1. Timefactor", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("1. Heigth", CKPGUID_FLOAT, "1");
    proto->DeclareInParameter("1. Wavelength", CKPGUID_FLOAT, "20");
    proto->DeclareInParameter("1. Sinpos", CKPGUID_VECTOR, "0.0,0.0,0.0");

    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);
    proto->DeclareLocalParameter("", CKPGUID_POINTER);

    proto->DeclareSetting("SinusCount", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTSinusMeshdeform);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(TTSinusMeshdeformCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTSinusMeshdeform(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTSinusMeshdeformCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}