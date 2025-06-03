/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Set Shade Mode
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetShadeModeDecl();
CKERROR CreateSetShadeModeProto(CKBehaviorPrototype **);
int SetShadeMode(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetShadeModeDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Shade Mode");
    od->SetDescription("Sets the Shade Mode of a Material");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>ShadeMode: </SPAN>Flat, Gouraud or Phong.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x95195195, 0x98795198));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetShadeModeProto);
    od->SetCompatibleClassId(CKCID_MATERIAL);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetShadeModeProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Shade Mode");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("ShadeMode", CKPGUID_SHADEMODE, "Flat");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetShadeMode);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetShadeMode(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKMaterial *mat = (CKMaterial *)beh->GetTarget();
    if (!mat)
        return CKBR_OWNERERROR;

    int mode = VXSHADE_FLAT;
    beh->GetInputParameterValue(0, &mode);

    mat->SetShadeMode((VXSHADE_MODE)mode);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
