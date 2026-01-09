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
    od->SetDescription("Creates light dependent bump mapping");
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
    CKContext *ctx = behcontext.Context;
    CKRenderContext *rc = behcontext.CurrentRenderContext;

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, TRUE);
    }

    CKCamera *camera = (CKCamera *)beh->GetInputParameterObject(0);
    CKScene *scene = ctx->GetCurrentScene();

    // Save current camera
    CKCamera *prevCamera = rc->GetAttachedCamera();

    // Attach custom camera if provided
    if (camera)
        rc->AttachViewpointToCamera(camera);

    // Trigger scene draw to calculate visibility
    rc->DrawScene(CK_RENDER_USECURRENTSETTINGS);

    // Iterate through scene objects
    XObjectPointerArray objectArray = scene->ComputeObjectList(CKCID_3DENTITY);
    for (CKObject **it = objectArray.Begin(); it != objectArray.End(); ++it)
    {
        CK3dEntity *entity = (CK3dEntity *)*it;
        if (entity)
        {
            // Check if entity is in view frustum
            if (entity->IsInViewFrustrum(rc, 0xFF))
                entity->Show(CKSHOW);
            else
                entity->Show(CKHIDE);
        }
    }

    // Restore original camera
    rc->AttachViewpointToCamera(prevCamera);

    return CKBR_ACTIVATENEXTFRAME;
}

CKERROR ViewFrustumClippingCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    if (behcontext.CallbackMessage == CKM_BEHAVIORPAUSE)
    {
        // Show all hidden objects when pausing
        CKScene *scene = ctx->GetCurrentScene();
        XObjectPointerArray objectArray = scene->ComputeObjectList(CKCID_3DENTITY);
        for (CKObject **it = objectArray.Begin(); it != objectArray.End(); ++it)
        {
            CK3dEntity *entity = (CK3dEntity *)*it;
            if (entity)
                entity->Show(CKSHOW);
        }
    }

    return CKBR_OK;
}