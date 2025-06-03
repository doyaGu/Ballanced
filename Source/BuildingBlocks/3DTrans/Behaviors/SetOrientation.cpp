/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Orientation
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetOrientationDecl();
CKERROR CreateSetOrientationProto(CKBehaviorPrototype **pproto);
int SetOrientation(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetOrientationDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Orientation");
    od->SetDescription("Sets the orientation of a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Dir: </SPAN>determines the Z local axis direction.<BR>
    <SPAN CLASS=pin>Up: </SPAN>determines the Y local axis so that the Y/Z plane is the same as Up/Dir.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <SPAN CLASS=pin>Referential: </SPAN>referential in which the 'Dir' and 'Up' vector are expressed (-NULL- in most cases).<BR>
    <SPAN CLASS=pin>Local Dir: </SPAN>determines the local vector that will be considered as Z axis.<BR>
    <SPAN CLASS=pin>Local Up: </SPAN>determines the local vector that will be considered as Y axis, so that the Y/Z plane is the same as Up/Dir.<BR>
    <BR>
    Well, your maybe not comfortable with trigonometry, and anyway orientation in 3D-Space is always a critical problem, so let's take an example :<BR>
    Say you want to orientate a gun toward a target.<BR>
    Calculating the direction vector 'Dir' (joining the gun and its target) is not enough to determine the orientation of the gun : even though the gun is pointing in the right direction, the handle might not be in the right position.<BR>
    We still need to determine the 'Up' vector. In this case, usinf the world 'Up' axis ensures that the gun will be properly oriented.<BR>
    Specifying the 'Dir' and 'Up' vector is enough to determine the orientation of a 3D model provided that this model's local axes follow the default coordinate system : 'X' --> Right ; 'Y'--> Up ; 'Z'--> Front (or Direction). Otherwise, you need to specify which local axes to consider as the 'Dir' and 'Up' axes of the object.<BR>
    <FONT SIZE=2>Picture 1 : </FONT>the object's Y axis is oriented along the object's 'Up' direction, and the object's Z axis is oriented along the object's aiming direction.<BR>
    You don't need to change the 'Local Dir', and 'Local Up' values.<BR>
    <IMG SRC="orient2.gif"><BR><P>
    <FONT SIZE=2>Picture 2 : </FONT>the object's local axes are not correctly set. You will need to specify 'Local Dir'=[0,1,0] and 'Local Up'=[0,0,1].<BR>
    <IMG SRC="orient1.gif"><BR>
    */
    /* See Also: 'Set Euler Orientation', 'Set World Matrix', 'Set Local Matrix', (and 'Interpolator Matrix', 'Interpolator Orientation' in the Logics/Interpolator building block category).<BR>
     */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x625874aa, 0xaa694132));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetOrientationProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetOrientationProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Orientation");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Dir", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Up", CKPGUID_VECTOR, "0,1,0");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Referential", CKPGUID_3DENTITY);
    proto->DeclareInParameter("Local Dir", CKPGUID_VECTOR, "0,0,1");
    proto->DeclareInParameter("Local Up", CKPGUID_VECTOR, "0,1,0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetOrientation);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetOrientation(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get dir
    VxVector dir(0, 0, 1);
    beh->GetInputParameterValue(0, &dir);

    // Get up
    VxVector up(0, 1, 0);
    beh->GetInputParameterValue(1, &up);

    // Hierarchy ?
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(2, &k);
    k = !k;

    // Referential
    CK3dEntity *ref = NULL;
    ref = (CK3dEntity *)beh->GetInputParameterObject(3);

    // Get Local dir
    VxVector ldir(0, 0, 1);
    beh->GetInputParameterValue(4, &ldir);

    // Get Local up
    VxVector lup(0, 1, 0);
    beh->GetInputParameterValue(5, &lup);

    float ps;
    //----------------------- Calculate the desired orientation (dir, real_up, right)
    VxVector right, real_up;

    dir = Normalize(dir);

    ps = up.x * dir.x + up.y * dir.y + up.z * dir.z;
    real_up.x = up.x - ps * dir.x;
    real_up.y = up.y - ps * dir.y;
    real_up.z = up.z - ps * dir.z;
    real_up = Normalize(real_up);

    right = CrossProduct(real_up, dir);

    //----------------------- Calculate the local desired orientation (ldir, lreal_up, lright)
    VxVector lright, lreal_up;

    ldir = Normalize(ldir);

    ps = lup.x * ldir.x + lup.y * ldir.y + lup.z * ldir.z;
    lreal_up.x = lup.x - ps * ldir.x;
    lreal_up.y = lup.y - ps * ldir.y;
    lreal_up.z = lup.z - ps * ldir.z;
    lreal_up = Normalize(lreal_up);

    lright = CrossProduct(lreal_up, ldir);

    //----------------------- Calculate the corresponding final transformation
    VxVector fdir, fup, fright;

    fright.x = lright.x * right.x + lreal_up.x * real_up.x + ldir.x * dir.x;
    fright.y = lright.x * right.y + lreal_up.x * real_up.y + ldir.x * dir.y;
    fright.z = lright.x * right.z + lreal_up.x * real_up.z + ldir.x * dir.z;

    fup.x = lright.y * right.x + lreal_up.y * real_up.x + ldir.y * dir.x;
    fup.y = lright.y * right.y + lreal_up.y * real_up.y + ldir.y * dir.y;
    fup.z = lright.y * right.z + lreal_up.y * real_up.z + ldir.y * dir.z;

    fdir.x = lright.z * right.x + lreal_up.z * real_up.x + ldir.z * dir.x;
    fdir.y = lright.z * right.y + lreal_up.z * real_up.y + ldir.z * dir.y;
    fdir.z = lright.z * right.z + lreal_up.z * real_up.z + ldir.z * dir.z;

    //  VxVector scale;
    //	ent->GetScale(&scale);

    ent->SetOrientation(&fdir, &fup, &fright, ref, k);
    //	ent->SetScale(&scale);

    return CKBR_OK;
}
