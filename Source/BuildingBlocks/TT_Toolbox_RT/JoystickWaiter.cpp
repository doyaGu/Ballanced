//////////////////////////////////////
//////////////////////////////////////
//
//        TT Joystick Waiter
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTJoystickWaiterBehaviorProto(CKBehaviorPrototype **pproto);
int TTJoystickWaiter(const CKBehaviorContext& behcontext);
CKERROR TTJoystickWaiterCallBack(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTJoystickWaiterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Joystick Waiter");
    od->SetDescription("Activates different outputs according to Joystick commands.");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x594a136e,0x58135ef9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateTTJoystickWaiterBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTJoystickWaiterBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Joystick Waiter");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("On");
    proto->DeclareInput("Off");

    proto->DeclareOutput("Joystick Idle");
    proto->DeclareOutput("Axis X Moved");
    proto->DeclareOutput("Axis Y Moved");

    proto->DeclareInParameter("Joystick Number", CKPGUID_INT, "0");

    proto->DeclareOutParameter("Analog Position", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareOutParameter("Rotation", CKPGUID_VECTOR, "0,0,0");
    proto->DeclareOutParameter("Sliders", CKPGUID_2DVECTOR, "-1.0,-1.0");
    proto->DeclareOutParameter("Point Of View", CKPGUID_ANGLE);
    proto->DeclareOutParameter("Sliders Diff", CKPGUID_FLOAT, "0.0");

    proto->DeclareSetting("Axis Sensitivity", CKPGUID_VECTOR, "0.5,0.1,0.5");
    proto->DeclareSetting("Axis Outputs", , "Axis X,Axis Y");
    proto->DeclareSetting("Button Outputs", CKPGUID_FILTER, "1");
    proto->DeclareSetting("Relative Moves", , "Slider 1,Slider 2,POV");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTJoystickWaiter);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetBehaviorCallbackFct(TTJoystickWaiterCallBack);

    *pproto = proto;
    return CK_OK;
}

int TTJoystickWaiter(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTJoystickWaiterCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}