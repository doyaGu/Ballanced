/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SetColorKey2d
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetColorKey2dDecl();
CKERROR CreateSetColorKey2dProto(CKBehaviorPrototype **pproto);
int SetColorKey2d(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetColorKey2dDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Color Key 2D");
    od->SetDescription("Sets the transparent transparence color key for a sprite.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Translucent Color: </SPAN>color expressed in RGBA.<BR>
    <SPAN CLASS=pin>Transparent: </SPAN>activates or deactivates the the translucent mode.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7bda540c, 0x39782384));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetColorKey2dProto);
    od->SetCompatibleClassId(CKCID_SPRITE);
    od->SetCategory("Visuals/2D");
    return od;
}

CKERROR CreateSetColorKey2dProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = NULL;
    proto = CreateCKBehaviorPrototype("Set Color Key 2D");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Translucide Color", CKPGUID_COLOR, "0,0,0,0");
    proto->DeclareInParameter("Transparent", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SetColorKey2d);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SetColorKey2d(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CKSprite *tex = (CKSprite *)beh->GetTarget();
    if (!tex)
        return CKBR_OWNERERROR;

    // we get a int (input parameter)
    VxColor col;
    beh->GetInputParameterValue(0, &col);

    tex->SetTransparentColor(RGBAFTOCOLOR(col.r, col.g, col.b, col.a));

    // we get a BOOL (input parameter)
    CKBOOL t;
    beh->GetInputParameterValue(1, &t);

    tex->SetTransparent(t);

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
