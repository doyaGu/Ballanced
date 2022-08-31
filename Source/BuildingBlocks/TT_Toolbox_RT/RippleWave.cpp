/////////////////////////////////
/////////////////////////////////
//
//        TT_RippleWave
//
/////////////////////////////////
/////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTRippleWaveBehaviorProto(CKBehaviorPrototype **pproto);
int TTRippleWave(const CKBehaviorContext& behcontext);
CKERROR TTRippleWaveCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTRippleWaveDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_RippleWave");
    od->SetDescription("erzeugt Wellenringe");
    od->SetCategory("TT Toolbox/Mesh");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2f6a47b3,0x35f4755c));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTRippleWaveBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTRippleWaveBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_RippleWave");
    if(!proto) return CKERR_OUTOFMEMORY;

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
    proto->SetFunction(TTRippleWave);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    proto->SetBehaviorCallbackFct(TTRippleWaveCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTRippleWave(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTRippleWaveCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}