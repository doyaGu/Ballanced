/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Bounding Sphere Intersection
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBoundingSphereIntersectionDecl();
CKERROR CreateBoundingSphereIntersectionProto(CKBehaviorPrototype **);
int BoundingSphereIntersection(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBoundingSphereIntersectionDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Sphere Sphere Intersection");
    od->SetDescription("Detects the collision between two 3D entities at the bounding sphere level.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>True: </SPAN>is activated if the 2 entities are in intersection.<BR>
    <SPAN CLASS=out>False: </SPAN>is activated if the 2 entities are not in intersection.<BR>
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
    od->SetGuid(CKGUID(0x198531f9, 0x60f66c57));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBoundingSphereIntersectionProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateBoundingSphereIntersectionProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Sphere Sphere Intersection");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("True");
    proto->DeclareOutput("False");

    proto->DeclareInParameter("Entity 0", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Entity 1", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Hierarchy 0", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Hierarchy 1", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BoundingSphereIntersection);

    *pproto = proto;
    return CK_OK;
}

int BoundingSphereIntersection(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // Set Input state
    beh->ActivateInput(0, FALSE);

    CK3dEntity *ent1 = (CK3dEntity *)beh->GetInputParameterObject(0);
    CK3dEntity *ent2 = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (!ent1 || !ent2)
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }

    CKBOOL eh1 = FALSE;
    beh->GetInputParameterValue(2, &eh1);
    CKBOOL eh2 = FALSE;
    beh->GetInputParameterValue(3, &eh2);

    float radius1 = 0.0f;
    float radius2 = 0.0f;

    VxVector pos1;
    VxVector pos2;

    VxBbox box;

    if (eh1)
    {
        box = ent1->GetHierarchicalBox();
        radius1 = 0.5f * Magnitude(box.Max - box.Min);
        pos1 = (box.Max + box.Min) * 0.5f;
    }
    else
    {
        // box = ent1->GetBoundingBox();
        radius1 = ent1->GetRadius();
        ent1->GetBaryCenter(&pos1);
    }

    if (eh2)
    {
        box = ent2->GetHierarchicalBox();
        radius2 = 0.5f * Magnitude(box.Max - box.Min);
        pos2 = (box.Max + box.Min) * 0.5f;
    }
    else
    {
        // box = ent2->GetBoundingBox();
        radius2 = ent2->GetRadius();
        ent2->GetBaryCenter(&pos2);
    }

    float dist = SquareMagnitude(pos2 - pos1);
    float radSum = radius1 + radius2;
    radSum *= radSum;

    if (radSum < dist)
    {
        beh->ActivateOutput(1);
    }
    else
    {
        beh->ActivateOutput(0);
    }

    return CKBR_OK;
}
