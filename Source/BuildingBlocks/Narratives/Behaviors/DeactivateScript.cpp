/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            DeactivateScript
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorDeactivateScriptDecl();
CKERROR CreateDeactivateScriptProto(CKBehaviorPrototype **);
int DeactivateScript(const CKBehaviorContext &behcontext);
CKERROR DeactivateScriptCallBack(const CKBehaviorContext &behcontext); // CallBack Function

CKObjectDeclaration *FillBehaviorDeactivateScriptDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Deactivate Script");
    od->SetDescription("Deactivates a Script.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Script: </SPAN>script to be deactivated.<BR>
    <BR>
    This building block allows scripts to be deactivated dynamically.<BR>
    By adding input parameters, as many scripts as required may be deactivated.<BR>
    See also: ActivateScript.<BR>
    */
    /* warning:
    - the script will actually be de-activated in the next frame.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x14367c05, 0x635b24f9));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDeactivateScriptProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Script Management");
    return od;
}

CKERROR CreateDeactivateScriptProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Deactivate Script");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Script", CKPGUID_SCRIPT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFunction(DeactivateScript);
    proto->SetBehaviorCallbackFct(DeactivateScriptCallBack);

    *pproto = proto;
    return CK_OK;
}

int DeactivateScript(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    for (int i = 0; i < beh->GetInputParameterCount(); ++i)
    {
        CKBehavior *script = (CKBehavior *)beh->GetInputParameterObject(i);
        behcontext.CurrentScene->DeActivate(script);
    }

    return CKBR_OK;
}

CKERROR DeactivateScriptCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        for (int i = 0; i < beh->GetInputParameterCount(); i++)
        {
            pin = beh->GetInputParameter(i);
            if (pin->GetGUID() != CKPGUID_SCRIPT)
            {
                pin->SetGUID(CKPGUID_SCRIPT);
            }
        }
    }
    }

    return CKBR_OK;
}
