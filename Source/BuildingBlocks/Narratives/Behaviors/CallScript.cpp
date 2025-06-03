/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            CallScript
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorCallScriptDecl();
CKERROR CreateCallScriptProto(CKBehaviorPrototype **);
int CallScript(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorCallScriptDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Call Script");
    od->SetDescription("Calls immediately a script.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the activated script's process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Reset?: </SPAN>if TRUE, the script will be reset; if FALSE the script will be resumed from its previous state.<BR>
    <SPAN CLASS=pin>Script: </SPAN>script to be performed.<BR>
    <SPAN CLASS=pin>Wait For Completion?: </SPAN>if TRUE, the script will be executed until completion; if FALSE the script will be executed for one frame.<BR>
    <BR>
    The script called is executed immediately, stopping the execution of the current script.
    */
    /* warning:
    The script must not contain a infinite loop if you set Wait For Completion.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1a7d5b17, 0x70630fed));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateCallScriptProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Script Management");
    return od;
}

CKERROR CreateCallScriptProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Call Script");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Reset ?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Script", CKPGUID_SCRIPT);
    proto->DeclareInParameter("Wait For Completion?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(CallScript);

    *pproto = proto;
    return CK_OK;
}

int CallScript(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    //----- Get Script
    CKBehavior *script = (CKBehavior *)beh->GetInputParameterObject(1);
    if (!script)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    //----- Get Reset Boolean
    CKBOOL reset = FALSE;
    beh->GetInputParameterValue(0, &reset);

    if (reset)
        script->Activate(FALSE, TRUE);

    // Get waitcomp
    CKBOOL wc = TRUE;
    beh->GetInputParameterValue(2, &wc);

    if (wc)
    {
        int loop = 0;
        int maxloop = behcontext.Context->GetBehaviorManager()->GetBehaviorMaxIteration();
        for (;; ++loop)
        {
            if (loop > maxloop)
            {
                behcontext.Context->OutputToConsoleExBeep("Execute Script : Script %s Executed too much times", script->GetName());
                script->Activate(FALSE, FALSE);
                break;
            }
            CKERROR result = script->Execute(behcontext.DeltaTime);
            // The script loop on itself too much times
            if (result == CKBR_INFINITELOOP)
            {
                // behcontext.Context->OutputToConsoleExBeep("Execute Script : Script %s Executed too much times",script->GetName());
                script->Activate(FALSE, FALSE);
                break;
            }
            if (!script->IsActive())
                break;
        }
    }
    else // we execute only one frame of the script
    {
        script->Execute(behcontext.DeltaTime);
        script->Activate(FALSE, FALSE);
    }

    // IO Activation
    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    return CKBR_OK;
}
