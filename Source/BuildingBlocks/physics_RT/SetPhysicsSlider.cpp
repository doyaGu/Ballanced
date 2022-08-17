/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Set Physics Slider
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "physics_RT.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorSetPhysicsSliderDecl();
CKERROR CreateSetPhysicsSliderProto(CKBehaviorPrototype **);
int SetPhysicsSlider(const CKBehaviorContext &behcontext);
CKERROR SetPhysicsSliderCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPhysicsSliderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Physics Slider");
    od->SetDescription("Set a Physics Sliders...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2973360E, 0x23D31AA7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("TERRATOOLS");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreateSetPhysicsSliderProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetPhysicsSliderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Physics Slider");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Destroy");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->DeclareInParameter("Object2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Axis first Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Axis second Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Limitations (meter)", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Lower Limit", CKPGUID_FLOAT, "-1.0");
    proto->DeclareInParameter("Upper Limit", CKPGUID_FLOAT, "1.0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetPhysicsSlider);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)CKBEHAVIOR_TARGETABLE);
    proto->SetBehaviorCallbackFct(SetPhysicsSliderCallBack);

    *pproto = proto;
    return CK_OK;
}

#define OBJECT2 0
#define AXIS_POINT1 1
#define AXIS_POINT2 2
#define LIMITATIONS 3
#define LOWER_LIMIT 4
#define UPPER_LIMIT 5

int SetPhysicsSlider(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *context = behcontext.Context;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
    {
        return CKBR_OWNERERROR;
    }

    CKIpionManager *man = CKIpionManager::GetManager(context);
    if (!man)
    {
        context->OutputToConsoleExBeep("TT_SetPhysicsSlider: pm==NULL.");
        return CKBR_OK;
    }

    // TODO

    return CKBR_OK;
}

CKERROR SetPhysicsSliderCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
