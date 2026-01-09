//////////////////////////////////////
//////////////////////////////////////
//
//        TT_TestForJoystick
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorTestForJoystickDecl();
CKERROR CreateTestForJoystickProto(CKBehaviorPrototype **pproto);
int TestForJoystick(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTestForJoystickDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_TestForJoystick");
    od->SetDescription("TT_TestForJoystick");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x229865eb, 0x577619e4));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTestForJoystickProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTestForJoystickProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_TestForJoystick");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Do");

    proto->DeclareOutput("Yes");
    proto->DeclareOutput("No");

    proto->DeclareInParameter("Nr", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TestForJoystick);

    *pproto = proto;
    return CK_OK;
}

int TestForJoystick(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    int joystickNr = 0;
    beh->GetInputParameterValue(0, &joystickNr);

    CKInputManager *inputManager = (CKInputManager *)ctx->GetManagerByGuid(INPUT_MANAGER_GUID);
    CKBOOL hasJoystick = inputManager->IsJoystickAttached(joystickNr);

    // Output 0 = Yes (joystick present), Output 1 = No (joystick not present)
    beh->ActivateOutput(hasJoystick ? 0 : 1, TRUE);
    beh->ActivateInput(0, FALSE);

    return CKBR_OK;
}