//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT CheckCurveCollision
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorCheckCurveCollisionDecl();
CKERROR CreateCheckCurveCollisionProto(CKBehaviorPrototype **pproto);
int CheckCurveCollision(const CKBehaviorContext &behcontext);
CKERROR CheckCurveCollisionCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorCheckCurveCollisionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT CheckCurveCollision");
    od->SetDescription("Switches the sequential stream to the corresponding ouput, following the result of comparison between 'Test' and the others input parameters");
    od->SetCategory("TT Toolbox/Collision");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7e9a4415, 0x199e4eb3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCheckCurveCollisionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateCheckCurveCollisionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT CheckCurveCollision");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Check");

    proto->DeclareOutput("TRUE");
    proto->DeclareOutput("FALSE");

    proto->DeclareInParameter("Curve Group", CKPGUID_GROUP);
    proto->DeclareInParameter("HierachieBB", CKPGUID_BOOL, "FALSE");

    proto->DeclareOutParameter("MinDistance", CKPGUID_FLOAT);

    proto->DeclareSetting("MinDistance", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CheckCurveCollision);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS | CKBEHAVIOR_INTERNALLYCREATEDOUTPUTPARAMS));
    proto->SetBehaviorCallbackFct(CheckCurveCollisionCallBack);

    *pproto = proto;
    return CK_OK;
}

int CheckCurveCollision(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR CheckCurveCollisionCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}