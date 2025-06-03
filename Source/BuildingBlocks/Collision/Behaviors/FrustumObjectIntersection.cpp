/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Box Box Intersection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorFrustumObjectIntersectionDecl();
CKERROR CreateFrustumObjectIntersectionProto(CKBehaviorPrototype **);
int FrustumObjectIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorFrustumObjectIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Frustum Object Intersection");
    od->SetDescription("Detects the collision between a camera frustum and an object.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if the camera frustum and the object are in intersection.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if the camera frustum and the object are not in intersection.<BR>
    <BR>
    <SPAN CLASS=pin>Entity 0: </SPAN>first entity to be tested.<BR>
    <SPAN CLASS=pin>Entity 1: </SPAN>second entity to be tested.<BR>
    <SPAN CLASS=pin>Hierarchy 0: </SPAN>specify whether to concider the bounding sphere of the first entity and its children, or just the bounding sphere of the first entity.<BR>
    <SPAN CLASS=pin>Hierarchy 1: </SPAN>specify whether to concider the bounding sphere of the second entity and its children, or just the bounding sphere of the second entity.<BR>
    <BR>
    This building block detects the collision between two entity's bounding spheres.<BR>
    */
    od->SetCategory("Collisions/Intersection");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4d6075e1, 0x2dda272e));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateFrustumObjectIntersectionProto);
    od->SetCompatibleClassId(CKCID_CAMERA);
    return od;
}

CKERROR CreateFrustumObjectIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Frustum Object Intersection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Object", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Local", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(FrustumObjectIntersection);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int FrustumObjectIntersection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Set Input state
    beh->ActivateInput(0, FALSE);

    CKCamera *cam = (CKCamera *)beh->GetTarget();
    if (!cam)
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CK3dEntity *ent = (CK3dEntity *)beh->GetInputParameterObject(0);
    if (!ent)
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CKBOOL local = FALSE;
    beh->GetInputParameterValue(1, &local);

    CKBOOL hiera = FALSE;
    beh->GetInputParameterValue(2, &hiera);

    int width, height;
    VxVector pos, dir, up, right;
    cam->GetPosition(&pos);
    cam->GetOrientation(&dir, &up, &right);
    cam->GetAspectRatio(width, height);
    VxFrustum frustum(pos, right, up, dir, cam->GetFrontPlane(), cam->GetBackPlane(), cam->GetFov(), (float)height / width);

    VxBbox box;
    if (local)
    {
        if (!hiera)
        {
            box = ent->GetBoundingBox(TRUE);
        }
        else
        {
            box = ent->GetHierarchicalBox(TRUE);
        }

        if (VxIntersect::FrustumOBB(frustum, box, ent->GetWorldMatrix()))
        {
            beh->ActivateOutput(0);
        }
        else
        {
            beh->ActivateOutput(1);
        }
    }
    else
    {
        if (!hiera)
        {
            box = ent->GetBoundingBox(FALSE);
        }
        else
        {
            box = ent->GetHierarchicalBox(FALSE);
        }

        if (VxIntersect::FrustumAABB(frustum, box))
        {
            beh->ActivateOutput(0);
        }
        else
        {
            beh->ActivateOutput(1);
        }
    }

    return CKBR_OK;
}
