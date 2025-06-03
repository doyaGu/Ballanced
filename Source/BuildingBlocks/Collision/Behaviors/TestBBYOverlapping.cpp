/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            TestBBYOverlapping
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorTestBBYOverlappingDecl();
CKERROR CreateTestBBYOverlappingProto(CKBehaviorPrototype **);
int TestBBYOverlapping(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorTestBBYOverlappingDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Test Bounding Box Y Overlapping");
    od->SetDescription("Tests the Y Relative Position of the bounding boxes of two 3D Entities, in the world coordinate system.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <BR>
    <SPAN CLASS=out>A above B: </SPAN>is activated if the bounding box of ObjectA is above that of ObjectB.<BR>
    <SPAN CLASS=out>B above A: </SPAN>is activated if the bounding box of ObjectB is above that of ObjectA.<BR>
    <SPAN CLASS=out>A above B overlapped: </SPAN>is activated if the bounding box of ObjectA is above that of ObjectB, but overlapping.<BR>
    <SPAN CLASS=out>B above A overlapped: </SPAN>is activated if the bounding box of ObjectB is above that of ObjectA, but overlapping.<BR>
    <SPAN CLASS=out>A and B overlapped: </SPAN>is activated if both bounding boxes are at the same level, overlapping.<BR>
    <BR>
    <SPAN CLASS=pin>Object A: </SPAN>first object to test.<BR>
    <SPAN CLASS=pin>Object B: </SPAN>second object to test.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x01ad010b, 0x01bd010b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTestBBYOverlappingProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("Collisions/3D Entity");
    return od;
}

CKERROR CreateTestBBYOverlappingProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Test Bounding Box Y Overlapping");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("A above B");
    proto->DeclareOutput("B above A");
    proto->DeclareOutput("A above B, overlapped");
    proto->DeclareOutput("B above A, overlapped");
    proto->DeclareOutput("A and B overlapped");

    proto->DeclareInParameter("ObjectA", CKPGUID_3DENTITY);
    proto->DeclareInParameter("ObjectB", CKPGUID_3DENTITY);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(TestBBYOverlapping);

    *pproto = proto;
    return CK_OK;
}

int TestBBYOverlapping(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    beh->ActivateInput(0, FALSE);

    // We Get the Object A
    CK3dEntity *ckA = (CK3dEntity *)beh->GetInputParameterObject(0);
    if (!ckA)
        return CKBR_OWNERERROR;
    const VxBbox &vxbbA = ckA->GetBoundingBox();

    // We Get the Obejct B
    CK3dEntity *ckB = (CK3dEntity *)beh->GetInputParameterObject(1);
    if (!ckB)
        return CKBR_OWNERERROR;
    const VxBbox &vxbbB = ckB->GetBoundingBox();

    if (vxbbA.Min.y > vxbbB.Max.y)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }
    if (vxbbB.Min.y > vxbbA.Max.y)
    {
        beh->ActivateOutput(1);
        return CKBR_OK;
    }
    if ((vxbbA.Min.y > vxbbB.Min.y) && (vxbbA.Max.y > vxbbB.Max.y))
    {
        beh->ActivateOutput(2);
        return CKBR_OK;
    }
    if ((vxbbB.Min.y > vxbbA.Min.y) && (vxbbB.Max.y > vxbbA.Max.y))
    {
        beh->ActivateOutput(3);
        return CKBR_OK;
    }
    beh->ActivateOutput(4);
    return CKBR_OK;
}
