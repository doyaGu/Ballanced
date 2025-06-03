/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ShowMouseCursor
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

extern CKObjectDeclaration *ODecl;

CKObjectDeclaration *FillBehaviorShowMouseCursorDecl();
CKERROR CreateShowMouseCursorProto(CKBehaviorPrototype **pproto);
int ShowMouseCursor(const CKBehaviorContext &behcontext);
CKERROR ShowMouseCursorCallBack(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorShowMouseCursorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Show Mouse Cursor");
    od->SetDescription("Show/Hide the mouse cursor.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Show: </SPAN>True shows the cursor, False hides it.<BR>
    <BR>
    */
    /* warning:
    - If you hide the cursor three times, you'll have to show it three times to actually have it back.<BR>
    <BR>
    */
    od->SetCategory("Visuals/Show-Hide");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x16f6368f, 0x506b60fc));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateShowMouseCursorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->NeedManager(INPUT_MANAGER_GUID);
    return od;
}

CKERROR CreateShowMouseCursorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Show Mouse Cursor");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Show", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ShowMouseCursor);
    //	proto->SetBehaviorCallbackFct(ShowMouseCursorCallBack);

    *pproto = proto;
    return CK_OK;
}

int ShowMouseCursor(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    // Set IO states
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0, TRUE);

    CKBOOL b = FALSE;
    beh->GetInputParameterValue(0, &b);

    CKInputManager *im = (CKInputManager *)behcontext.Context->GetManagerByGuid(INPUT_MANAGER_GUID);

    if (im)
        im->ShowCursor(b);

    return CKBR_OK;
}
