//////////////////////////////////////
//////////////////////////////////////
//
//        TT_TestForJoystick
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTTestForJoystickBehaviorProto(CKBehaviorPrototype **pproto);
int TTTestForJoystick(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTTestForJoystickDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_TestForJoystick");
    od->SetDescription("TT_TestForJoystick");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x229865eb,0x577619e4));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTTestForJoystickBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTTestForJoystickBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_TestForJoystick");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Do");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Nr", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTTestForJoystick);

    *pproto = proto;
    return CK_OK;
}

int TTTestForJoystick(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTTestForJoystickCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}