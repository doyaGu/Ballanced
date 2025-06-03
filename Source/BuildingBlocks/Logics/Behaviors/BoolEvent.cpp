/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            BoolEvent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorBoolEventDecl();
CKERROR CreateBoolEventProto(CKBehaviorPrototype **);
int BoolEvent(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorBoolEventDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Bool Event");
    od->SetDescription("Detects a boolean value change.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>Reset: </SPAN>resets the internal state memorized by the building block.<BR>
    The internal state is then set to FALSE, and none of the output is activated.<BR>
    <BR>
    <SPAN CLASS=out>Change Detected: </SPAN>is activated when 'Value' has changed (either from TRUE to FALSE, or from FALSE to TRUE).<BR>
    <SPAN CLASS=out>No Change: </SPAN>is activated when 'Value' has remained unchanged.<BR>
    <BR>
    <SPAN CLASS=pin>Value: </SPAN>boolean value.<BR>
    <BR>
    If the boolean value has changed since the last processing of the building block, the "Change Detected" output is activated, otherwise the "No Change" output is activated.<BR>
    */
    /* warning:
    - after a "Restore Initial Condition" event, the inner state of the building block isn't reseted.
    The only way to reset the building block is to activate its "Reset" input.<BR>
    - you can activate at same time "In" and "Reset" (the building block is then reset and the "In" input is activated).<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xfefefefe, 0xfefefefe));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateBoolEventProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Logics/Streaming");
    return od;
}

CKERROR CreateBoolEventProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Bool Event");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Reset");

    proto->DeclareOutput("Change Detected");
    proto->DeclareOutput("No Change");

    proto->DeclareInParameter("Value", CKPGUID_BOOL, "FALSE");

    proto->DeclareLocalParameter(NULL, CKPGUID_BOOL, "FALSE"); // "Old Value"

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(BoolEvent);

    *pproto = proto;
    return CK_OK;
}

int BoolEvent(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    //______________________________/ Enter by RESET
    if (beh->IsInputActive(1))
    {
        beh->ActivateInput(1, FALSE);

        CKBOOL old_k = FALSE;
        beh->SetLocalParameterValue(0, &old_k);
    }

    //______________________________/ Enter by IN
    if (beh->IsInputActive(0))
    {

        beh->ActivateInput(0, FALSE);

        CKBOOL k = FALSE;
        beh->GetInputParameterValue(0, &k);

        CKBOOL old_k = FALSE;
        beh->GetLocalParameterValue(0, &old_k);

        if (k != old_k)
            beh->ActivateOutput(0);
        else
            beh->ActivateOutput(1);

        beh->SetLocalParameterValue(0, &k);
    }

    return CKBR_OK;
}
