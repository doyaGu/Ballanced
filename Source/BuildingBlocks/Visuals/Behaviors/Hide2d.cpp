/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            Hide 2dEntity
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorHide2dDecl();
CKERROR CreateHide2dProto(CKBehaviorPrototype **pproto);
int Hide2d(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorHide2dDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Hide 2D Entity");
    od->SetDescription("Hides a 2D Entity");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    See Also: Show 2D Entity.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x13579753, 0x13579753));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateHide2dProto);
    od->SetCompatibleClassId(CKCID_2DENTITY);
    od->SetCategory("Visuals/Show-Hide");
    return od;
}

CKERROR CreateHide2dProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Hide 2D Entity");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_OBSOLETE);
    proto->SetFunction(Hide2d);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int Hide2d(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);

    beh->ActivateOutput(0, TRUE);

    CK2dEntity *ent = (CK2dEntity *)beh->GetTarget();
    if (!ent)
        return CKBR_OWNERERROR;

    ent->Show(CKHIDE);

    return CKBR_OK;
}
