//////////////////////////////////////////
//////////////////////////////////////////
//
//        TT_ViewFrustumClipping
//
//////////////////////////////////////////
//////////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorViewFrustumClippingDecl();
CKERROR CreateViewFrustumClippingProto(CKBehaviorPrototype **pproto);
int ViewFrustumClipping(const CKBehaviorContext &behcontext);
CKERROR ViewFrustumClippingCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorViewFrustumClippingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_ViewFrustumClipping");
    od->SetDescription("erzeugt lichtabh�ngiges BumpMapping");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x412e4cd0, 0x6f860de3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateViewFrustumClippingProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateViewFrustumClippingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_ViewFrustumClipping");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Camera", CKPGUID_CAMERA);

    proto->DeclareLocalParameter("MatChannel", CKPGUID_INT, "0");
    proto->DeclareLocalParameter("", CKPGUID_VOIDBUF);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ViewFrustumClipping);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetBehaviorCallbackFct(ViewFrustumClippingCallBack);

    *pproto = proto;
    return CK_OK;
}

int ViewFrustumClipping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR ViewFrustumClippingCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}