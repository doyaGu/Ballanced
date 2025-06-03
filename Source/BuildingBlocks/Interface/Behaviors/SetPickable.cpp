/////////////////////////////////////////////////////
//		            Set Pickable
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSetPickableDecl();
CKERROR CreateSetPickableProto(CKBehaviorPrototype **pproto);
int SetPickable(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorSetPickableDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set Pickable");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Pickable: </SPAN>if TRUE, the Render Object (3d entity, sprite, etc...) can be picked.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSetPickableProto);
    od->SetDescription("Sets the pickable state of a 3D Entity");
    od->SetCategory("Interface/Screen");
    od->SetGuid(CKGUID(0xF1499052, 0xEBE9BBF1));
    od->SetAuthorGuid(CKGUID(0x56495254, 0x4f4f4c53));
    od->SetAuthorName("Virtools");
    od->SetCompatibleClassId(CKCID_RENDEROBJECT);

    return od;
}

CKERROR CreateSetPickableProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set Pickable");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Pickable", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));

    proto->SetFunction(SetPickable);

    *pproto = proto;
    return CK_OK;
}

int SetPickable(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKRenderObject *ro = (CKRenderObject *)beh->GetTarget();
    if (!ro)
        return CKBR_OK;

    CKBOOL pick = FALSE;
    beh->GetInputParameterValue(0, &pick);

    if (CKIsChildClassOf(ro, CKCID_3DENTITY))
        ((CK3dEntity *)ro)->SetPickable(pick);
    else
        ((CK2dEntity *)ro)->SetPickable(pick);

    return CKBR_OK;
}
