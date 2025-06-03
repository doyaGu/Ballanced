/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		          Look At 3D Position
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorLookAtPosDecl();
CKERROR CreateLookAtPosProto(CKBehaviorPrototype **pproto);
int LookAtPos(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorLookAtPosDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Look At Pos");
    od->SetDescription("OBSOLETE - please see 'Look At' instead.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Target Position: </SPAN>vector position to look at.<BR>
    <SPAN CLASS=pin>Following Speed: </SPAN>how fast the 3D Entity should take to look at the given
    position. The higher the percentage, the faster the 3D Entity will turn to look.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN> if TRUE, then this behavior will also apply to the 3D Entity's children.<BR>
    <BR>
    As you can do the same and better with 'LookAt' building block, the 'LookAtPos' building block is an obsolete one.<BR>
    This building block needs to be looped if you want to constantly look at the target.<BR>
    <BR>
    See also : Look At.
    */
    /* warning:
    This building block is obsolete and is only included for backwards compatibility. Look At should be used instead.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xee6e5e6e, 0xe6e6e6e6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateLookAtPosProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    // ajout pour qu'on puisse avoir acces a la doc depuis la schematique
    od->SetCategory("3D Transformations/Constraint");
    return od;
}

CKERROR CreateLookAtPosProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Look At Pos");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Target Position", CKPGUID_VECTOR);
    proto->DeclareInParameter("Following Speed", CKPGUID_PERCENTAGE, "100");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(LookAtPos);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int LookAtPos(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // position of the owner object
    VxVector tmp1;
    ent->GetPosition(&tmp1);

    // position of object to look to
    VxVector tmp2;
    beh->GetInputParameterValue(0, &tmp2);

    // Effect Value.
    float value = 1.0f;
    beh->GetInputParameterValue(1, &value);

    // Stick Children ?
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(2, &k);
    k = !k;

    VxVector dir, up, right;
    dir = tmp2 - tmp1;

    if ((dir.x != 0) || (dir.y != 0) || (dir.z != 0))
    {

        dir = Normalize(dir);

        VxMatrix mat = ent->GetWorldMatrix();

        VxVector old_dir;
        old_dir.x = mat[2][0];
        old_dir.y = mat[2][1];
        old_dir.z = mat[2][2];
        old_dir = Normalize(old_dir);

        dir.x = old_dir.x + value * (dir.x - old_dir.x);
        dir.y = old_dir.y + value * (dir.y - old_dir.y);
        dir.z = old_dir.z + value * (dir.z - old_dir.z);

        up.x = 0.0f;
        up.y = 1.0f;
        up.z = 0.0f;
        right = CrossProduct(up, dir);
        up = CrossProduct(dir, right);

        dir = Normalize(dir);
        right = Normalize(right);
        up = Normalize(up);

        VxVector scale;
        ent->GetScale(&scale);
        ent->SetOrientation(&dir, &up, &right, NULL, k);
        ent->SetScale(&scale);

        /*
        VxMatrix matA;
        ent->GetWorldMatrix(matA);
        VxVector scale;
        ent->GetScale( &scale );
        ent->SetOrientation(&dir , &up, &right, NULL, k);


            ///////////////////// QUATERNION INTERPOLATION //////////////////

              VxQuaternion quatA, quatB, quatC;
              VxMatrix matB, matC;

                ent->GetWorldMatrix(matB);

                  quatA.FromMatrix(matA);
                  quatB.FromMatrix(matB);
                  quatC.Slerp(value, &quatA, &quatB); // Whooaa ... this is the amazing interpolation !
                  quatC.ToMatrix(matC);

                    ent->SetWorldMatrix(matC, k);
                    ent->SetScale( &scale );
                    ent->SetPosition(&tmp1, NULL);
        */

        // mov->SetOrientation(&dir , &up, &right, NULL, k);
        // ent->GetVxMoveable()->LookAt(&tmp, &tmp2, 0.0f, vxe);
    }
    return CKBR_OK;
}
