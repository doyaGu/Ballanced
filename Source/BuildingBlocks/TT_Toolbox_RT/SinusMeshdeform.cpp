//////////////////////////////////////
//////////////////////////////////////
//
//        TT SinusMeshdeform
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorSinusMeshdeformDecl();
CKERROR CreateSinusMeshdeformProto(CKBehaviorPrototype **pproto);
int SinusMeshdeform(const CKBehaviorContext &behcontext);
CKERROR SinusMeshdeformCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSinusMeshdeformDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT SinusMeshdeform");
    od->SetDescription("generiert Sinuswellen");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4f907f12, 0x16af15d6));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSinusMeshdeformProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSinusMeshdeformProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT SinusMeshdeform");
    if (!proto) return CKERR_OUTOFMEMORY;

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
    proto->SetFunction(SinusMeshdeform);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SinusMeshdeformCallBack);

    *pproto = proto;
    return CK_OK;
}

int SinusMeshdeform(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR SinusMeshdeformCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}