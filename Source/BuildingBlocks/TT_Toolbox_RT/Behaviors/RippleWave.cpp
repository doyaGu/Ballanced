/////////////////////////////////
/////////////////////////////////
//
//        TT_RippleWave
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorRippleWaveDecl();
CKERROR CreateRippleWaveProto(CKBehaviorPrototype **pproto);
int RippleWave(const CKBehaviorContext &behcontext);
CKERROR RippleWaveCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRippleWaveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_RippleWave");
    od->SetDescription("Creates wavy rings");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2f6a47b3, 0x35f4755c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRippleWaveProto);
    od->SetCompatibleClassId(CKCID_3DOBJECT);
    return od;
}

CKERROR CreateRippleWaveProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_RippleWave");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Drop");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Exit In");
    proto->DeclareOutput("Exit Off");

    proto->DeclareInParameter("DropStrength", CKPGUID_FLOAT, "1.0f");
    proto->DeclareInParameter("Viscosity", CKPGUID_FLOAT, "0.98f");
    proto->DeclareInParameter("Speed", CKPGUID_FLOAT, "0.25f");

    proto->DeclareLocalParameter("", CKPGUID_POINTER);
    proto->DeclareLocalParameter("Vertex Array", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RippleWave);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(RippleWaveCallBack);

    *pproto = proto;
    return CK_OK;
}

int RippleWave(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR RippleWaveCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}