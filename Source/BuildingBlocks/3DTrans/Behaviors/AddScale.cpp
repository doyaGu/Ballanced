/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            AddScale
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAddScaleDecl();
CKERROR CreateAddScaleProto(CKBehaviorPrototype **pproto);
int AddScale(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorAddScaleDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Scale");
    od->SetDescription("Scales the 3D Entity (relatively to its current size or not).");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Scale Vector: </SPAN>vector that defines the scale multiplication on the 3D Entity's X, Y and Z local axis.<BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also applies to the 3D Entity's children.<BR>
    <SPAN CLASS=pin>Absolute: </SPAN>if TRUE, the scale won't be relative to the current object's size, but absolute in the world (like the obsolete "Set Scale" BB).<BR>
    <BR>
    For instance, whatever the initial scale vector of the object should be, a Scaling Vector with (1,2,2) values, doubles the current size on the Y and Z local axis, but doesn't change the size along the X local axis.<BR>
    <BR>
    See also: Set Scale.<BR>
    */
    od->SetCategory("3D Transformations/Basic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x41236987, 0xa54a87a6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAddScaleProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateAddScaleProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Scale");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Scaling Vector", CKPGUID_VECTOR, "1,1,1");
    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Absolute", CKPGUID_BOOL, "FALSE");

    proto->SetFunction(AddScale);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int AddScale(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *ent = (CK3dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    // Get scaling vector
    VxVector v(1, 1, 1);
    beh->GetInputParameterValue(0, &v);

    // Scale Children
    CKBOOL k = TRUE;
    beh->GetInputParameterValue(1, &k);
    k = !k;

    // Absolute scale
    CKBOOL abs_scale = FALSE;
    beh->GetInputParameterValue(2, &abs_scale);

    // Is it a 3D Sprite ?
    if (CKIsChildClassOf(ent, CKCID_SPRITE3D))
    {
        Vx2DVector v2d;
        if (abs_scale)
        { // absolute scale
            v2d.x = v.x;
            v2d.y = v.y;
        }
        else
        { // relative scale
            ((CKSprite3D *)ent)->GetSize(v2d);
            v2d.x *= v.x;
            v2d.y *= v.y;
        }
        ((CKSprite3D *)ent)->SetSize(v2d);
    }
    else
    {

        if (abs_scale)
        { // absolute scale
            ent->SetScale(&v, k);
        }
        else
        { // relative scale
            ent->AddScale(&v, k);
        }
    }

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}