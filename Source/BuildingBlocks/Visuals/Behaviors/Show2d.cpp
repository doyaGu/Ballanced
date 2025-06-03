/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Show 2dEntity
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorShow2dDecl();
CKERROR CreateShow2dProto(CKBehaviorPrototype **pproto);
int Show2d(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorShow2dDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Show 2D Entity");
    od->SetDescription("Shows a 2D Entity");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    See Also: Hide 2D Entity.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xababa123, 0x123ababa));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShow2dProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    od->SetCategory("Visuals/Show-Hide");
    return od;
}

CKERROR CreateShow2dProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Show 2D Entity");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(Show2d);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Show2d(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CK2dEntity *ent = (CK2dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    ent->Show();

    return CKBR_OK;
}
