/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetColorKey
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetColorKeyDecl();
CKERROR CreateSetColorKeyProto(CKBehaviorPrototype **);
int SetColorKey(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetColorKeyDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Color Key");
    od->SetDescription("Sets the transparent color key of the selected texture.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Translucent Color: </SPAN>the color you want to be invisible.<BR>
    <SPAN CLASS=pin>Transparent: </SPAN>if TRUE, the chosen color will appear translucent on the texture.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x41bb5af1, 0x1eb26e9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetColorKeyProto);
    od->SetCompatibleClassId(CKCID_TEXTURE);
    od->SetCategory("Materials-Textures/Basic");
    return od;
}

CKERROR CreateSetColorKeyProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Color Key");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Translucent Color", CKPGUID_COLOR, "0,0,0,0");
    proto->DeclareInParameter("Transparent", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetColorKey);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetColorKey(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKTexture *tex = (CKTexture *)beh->GetTarget();
    if (!tex)
        return CKBR_OWNERERROR;

    // we get a int (input parameter)
    VxColor col(0, 0, 0);
    beh->GetInputParameterValue(0, &col);

    tex->SetTransparentColor(RGBAFTOCOLOR(col.r, col.g, col.b, col.a));

    // we get a BOOL (input parameter)
    CKBOOL t = TRUE;
    beh->GetInputParameterValue(1, &t);

    tex->SetTransparent(t);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
