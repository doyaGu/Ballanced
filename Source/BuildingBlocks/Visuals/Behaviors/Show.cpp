/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Show
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorShowDecl();
CKERROR CreateShowProto(CKBehaviorPrototype **pproto);
int Show(const CKBehaviorContext &behcontext);
void ShowHierarchy(CKBeObject *ent, CK_OBJECT_SHOWOPTION b);

CKObjectDeclaration *FillBehaviorShowDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Show");
    od->SetDescription("Shows a 2D/3D Entity, a Mesh or a Group");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Hierarchy: </SPAN>if TRUE, then this building block will also apply to the 3D Entity's children.<BR>
    <BR>
    See Also: Hide.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xa85a213a, 0xef78d52a));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShowProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Visuals/Show-Hide");
    return od;
}

CKERROR CreateShowProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Show");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hierarchy", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(Show);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Show(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    // Stick children ?
    CKBOOL k = FALSE;
    beh->GetInputParameterValue(0, &k);

    CKBeObject *beo = beh->GetTarget();
    if (!beo)
        return CKBR_OWNERERROR;

    if (k)
    {
        ShowHierarchy(beo, CKSHOW);
    }
    else
    {
        beo->Show(CKSHOW);
    }

    return CKBR_OK;
}
