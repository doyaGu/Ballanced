/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Scale
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetScaleDecl();
CKERROR CreateSetScaleProto(CKBehaviorPrototype **pproto);
int SetScale(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetScaleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Scale");
    od->SetDescription("Sets the Scale of a 3D Entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Scale Vector: </SPAN>vector that defines the scale of the 3D Entity on its X, Y and Y local axis.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    A scale vector [1, 1, 1] sets the 3D Entity to its initial unit modelling size.<BR>
    You can also scale a 3D Sprite.<BR>
    See Also: Add Scale.<BR>
    */
    /* warning:
    - If the object has been modellized and then scaled, setting its scale to [1,1,1] could reduce or enlarge it.
    - The "Set Scale" BB is now obsolete as it can be replace by the "Scale" BB if you set the pIn "Absolute" of this
    building block to TRUE.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7a7a7a7a, 0x7a7a7a7a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010001);
    od->SetCreationFunction(CreateSetScaleProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateSetScaleProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Scale");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Scale Vector", CKPGUID_VECTOR, "2,2,2");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(SetScale);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetScale(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    // Get the scaling vector
    VxVector vector(1, 1, 1);
    beh->GetInputParameterValue(0, &vector);

    // Stick children ?
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(1, &k);
    k = !k;

    // Is it a 3D Sprite
    if (CKIsChildClassOf(ent, CKCID_SPRITE3D))
    {
        Vx2DVector v2d(vector.x, vector.y);
        ((CKSprite3D *)ent)->SetSize(v2d);
    }
    else
    {
        ent->SetScale(&vector, k);
    }

    return CKBR_OK;
}
