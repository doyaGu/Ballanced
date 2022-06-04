/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TT SpeedOMeter
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "TT_Gravity_RT.h"

CKObjectDeclaration *FillBehaviorSpeedOMeterDecl();
CKERROR CreateSpeedOMeterProto(CKBehaviorPrototype **);
int SpeedOMeter(const CKBehaviorContext &behcontext);
CKERROR SpeedOMeterCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSpeedOMeterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT SpeedOMeter");
    od->SetDescription("Calculates the speed of an 3DEntity.");
    od->SetCategory("TT Gravity");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x51BD5521, 0x672C67BF));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSpeedOMeterProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSpeedOMeterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT SpeedOMeter");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Absolute Speed", CKPGUID_FLOAT, "0.0");

    proto->DeclareLocalParameter("Last Position", CKPGUID_VECTOR);
    proto->DeclareSetting("Calculate relative speed?", CKPGUID_BOOL, "0");
    proto->DeclareSetting("Loop-Framedelay?", CKPGUID_INT, "1");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SpeedOMeter);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SpeedOMeterCallBack);

    *pproto = proto;
    return CK_OK;
}

int SpeedOMeter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;
}

CKERROR SpeedOMeterCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    default:
        break;
    }

    return CKBR_OK;
}
