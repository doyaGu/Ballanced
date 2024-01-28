/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Set Physics Slider
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

#include "CKIpionManager.h"

CKObjectDeclaration *FillBehaviorSetPhysicsSliderDecl();
CKERROR CreateSetPhysicsSliderProto(CKBehaviorPrototype **pproto);
int SetPhysicsSlider(const CKBehaviorContext &behcontext);
CKERROR SetPhysicsSliderCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPhysicsSliderDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Physics Slider");
    od->SetDescription("Set a Physics Slider...");
    od->SetCategory("Physics");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x2973360e, 0x23d31aa7));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreateSetPhysicsSliderProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetPhysicsSliderProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Physics Slider");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Create");
    proto->DeclareInput("Shutdown");

    proto->DeclareOutput("Out1");
    proto->DeclareOutput("Out2");

    proto->DeclareInParameter("Object2", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Axis first Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Axis second Point", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Limitations (meter)", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Lower Limit", CKPGUID_FLOAT, "-1.0");
    proto->DeclareInParameter("Upper Limit", CKPGUID_FLOAT, "1.0");

    proto->DeclareLocalParameter("IVP_Handle", CKPGUID_POINTER);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetPhysicsSlider);

    proto->SetBehaviorFlags((CKBEHAVIOR_TARGETABLE);
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

    // TODO

    return CKBR_OK;
}

CKERROR SetPhysicsSliderCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // TODO

    return CKBR_OK;
}
