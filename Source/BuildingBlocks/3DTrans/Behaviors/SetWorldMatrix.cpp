/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		           Set World Matrix
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetWorldMatrixDecl();
CKERROR CreateSetWorldMatrixProto(CKBehaviorPrototype **pproto);
int SetWorldMatrix(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetWorldMatrixDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set World Matrix");
    od->SetDescription("Sets the World Matrix of a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>World Matrix: </SPAN>4x4 matrix.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    An object World Matrix determines its orientation and position with regard to the world basis.<BR>
    */
    /* warning:
    - Matrix is edited as the instantaneous matrix of a selected 3d Entity ( kind of snapshot of the selected object orientation when it is picked in the dialog box).
    - The Matrix value won't change during play mode unless you use the building block as low-level building block, where the matrix parameter is a result of other building blocks and/or parameter operations.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xaa4aa6f0, 0xddefdef4));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetWorldMatrixProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetWorldMatrixProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set World Matrix");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("World Matrix", CKPGUID_MATRIX);
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetWorldMatrix);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetWorldMatrix(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get matrix
    VxMatrix mat;
    beh->GetInputParameterValue(0, mat);

    /*
      // Test the correctness of the matrix
      if(!mat[0][0]) mat[0][0] = CK_ZERO;
      if(!mat[0][1]) mat[0][1] = CK_ZERO;
      if(!mat[0][2]) mat[0][2] = CK_ZERO;
      if(!mat[0][3]) mat[0][3] = CK_ZERO;
      if(!mat[1][0]) mat[1][0] = CK_ZERO;
      if(!mat[1][1]) mat[1][1] = CK_ZERO;
      if(!mat[1][2]) mat[1][2] = CK_ZERO;
      if(!mat[1][3]) mat[1][3] = CK_ZERO;
      if(!mat[2][0]) mat[2][0] = CK_ZERO;
      if(!mat[2][1]) mat[2][1] = CK_ZERO;
      if(!mat[2][2]) mat[2][2] = CK_ZERO;
      if(!mat[2][3]) mat[2][3] = CK_ZERO;
    */

    // Hierarchy
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(1, &k);
    k = !k;

    ent->SetWorldMatrix(mat, k);

    return CKBR_OK;
}
