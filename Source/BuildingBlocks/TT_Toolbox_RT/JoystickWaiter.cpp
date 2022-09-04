//////////////////////////////////////
//////////////////////////////////////
//
//        TT Joystick Waiter
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorJoystickWaiterDecl();
CKERROR CreateJoystickWaiterProto(CKBehaviorPrototype **pproto);
int JoystickWaiter(const CKBehaviorContext &behcontext);
CKERROR JoystickWaiterCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorJoystickWaiterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Joystick Waiter");
    od->SetDescription("Activates different outputs according to Joystick commands.");
    od->SetCategory("TT Toolbox/Controllers");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x594a136e, 0x58135ef9));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00020000);
    od->SetCreationFunction(CreateJoystickWaiterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateJoystickWaiterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Joystick Waiter");
    if (!proto) return CKERR_OUTOFMEMORY;

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
    proto->DeclareSetting("Axis Outputs", CKPGUID_JOYAXIS, "Axis X,Axis Y");
    proto->DeclareSetting("Button Outputs", CKPGUID_FILTER, "1");
    proto->DeclareSetting("Relative Moves", CKPGUID_JOYAXIS, "Slider 1,Slider 2,POV");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(JoystickWaiter);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS));
    proto->SetBehaviorCallbackFct(JoystickWaiterCallBack);

    *pproto = proto;
    return CK_OK;
}

int JoystickWaiter(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR JoystickWaiterCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}