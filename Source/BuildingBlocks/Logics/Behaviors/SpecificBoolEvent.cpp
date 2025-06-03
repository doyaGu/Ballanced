/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SpecificBoolEvent
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSpecificBoolEventDecl();
CKERROR CreateSpecificBoolEventProto(CKBehaviorPrototype **);
int SpecificBoolEvent(const CKBehaviorContext &behcontext);

CKERROR TriggerEventCallBack(const CKBehaviorContext &behcontext); // CallBack Functioon

CKObjectDeclaration *FillBehaviorSpecificBoolEventDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Specific Bool Event");
    od->SetDescription("Detects a boolean value change. Same as 'BoolEvent', but with specific outputs.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=in>Reset: </SPAN>resets the internal state memorized by the building block.<BR>
    The internal state is then set to FALSE, and none of the output is activated.<BR>
    <BR>
    <SPAN CLASS=out>True -> False: </SPAN>is activated when "Value" has changed from True to False.<BR>
    <SPAN CLASS=out>False -> True: </SPAN>is activated when "Value" has changed from False to True.<BR>
    <SPAN CLASS=out>Still True: </SPAN>is activated when "Value" has remained True.<BR>
    <SPAN CLASS=out>Still False: </SPAN>is activated when "Value" has remained False.<BR>
    <BR>
    <SPAN CLASS=pin>Value: </SPAN>boolean value.
    */
    /* warning:
    - you can activate at same time "In" and "Reset" (the building block is then reset and the "In" input is activated).<BR>
    */
    od->SetCategory("Logics/Streaming");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xe5f22514, 0x12dfffee));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSpecificBoolEventProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateSpecificBoolEventProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Specific Bool Event");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareInput("Reset");

    proto->DeclareOutput("True -> False");
    proto->DeclareOutput("False -> True");
    proto->DeclareOutput("Still True");
    proto->DeclareOutput("STill False");

    proto->DeclareInParameter("Value", CKPGUID_BOOL, "FALSE");

    proto->DeclareLocalParameter(NULL, CKPGUID_BOOL, "FALSE"); // "Old Value"

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SpecificBoolEvent);
    proto->SetBehaviorCallbackFct(TriggerEventCallBack);

    *pproto = proto;
    return CK_OK;
}

int SpecificBoolEvent(const CKBehaviorContext &behcontext)
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

        if (old_k && !k)
            beh->ActivateOutput(0);
        else if (!old_k && k)
            beh->ActivateOutput(1);
        else if (old_k && k)
            beh->ActivateOutput(2);
        else
            beh->ActivateOutput(3);

        beh->SetLocalParameterValue(0, &k);
    }

    return CKBR_OK;
}
