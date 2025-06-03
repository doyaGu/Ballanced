/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Set Local Matrix
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetLocalMatrixDecl();
CKERROR CreateSetLocalMatrixProto(CKBehaviorPrototype **pproto);
int SetLocalMatrix(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetLocalMatrixDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Local Matrix");
    od->SetDescription("Sets the Local Matrix of the 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Local Matrix: </SPAN>4x4 local matrix.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    An object Local Matrix determines its orientation and position with regard to its parent's basis.<BR>
    Which means for example, if the parent object moves, the child's local matrix will not necessarily be afected.<BR>
    */
    /* warning:
    - Matrix is edited as the instantaneous matrix of a selected 3d Entity (kind of snapshot of the selected object orientation when it is picked in the dialog box).
    - The Matrix value won't change during play mode unless you use the building block as low-level building block, where the matrix parameter is a result of other building blocks and/or parameter operations.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x21f5f30d, 0x08d5a1db));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetLocalMatrixProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetLocalMatrixProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Local Matrix");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Local Matrix", CKPGUID_MATRIX);
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetLocalMatrix);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetLocalMatrix(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get matrix
    VxMatrix mat;
    beh->GetInputParameterValue(0, mat);

    // Hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(1, &k);
    k = !k;

    ent->SetLocalMatrix(mat, k);

    return CKBR_OK;
}
