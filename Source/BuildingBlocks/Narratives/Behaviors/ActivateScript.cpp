/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ActivateScript
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorActivateScriptDecl();
CKERROR CreateActivateScriptProto(CKBehaviorPrototype **);
int ActivateScript(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorActivateScriptDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Activate Script");
    od->SetDescription("Activates a script, either in the state it was before deactivation or once reset.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Reset?: </SPAN>if TRUE, the script will be reset; if FALSE the script will carry on from the state it was in before being deactivated.<BR>
    <SPAN CLASS=pin>Script: </SPAN>script to be activated.<BR>
    <BR>
    <SPAN CLASS=setting>Awake Object: </SPAN>activate the object if it was inactive.<BR>
    <BR>
    With this building block scripts can be divided up into several smaller scripts and re-activated as needed.<BR>
    By adding input parameters, as many scripts as needed may be re-activated.<BR>
    See Also: Deactivate Script.<BR>
    */
    /* warning:
    - the activation will be done at the next frame.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4c7e7bc3, 0x0b693155));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateActivateScriptProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Script Management");
    return od;
}

CKERROR ActivateScriptCallBack(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    switch (behcontext.CallbackMessage)
    {
    case CKM_BEHAVIOREDITED:
    {
        CKParameterIn *pin;
        pin = beh->GetInputParameter(0);
        if (pin->GetGUID() == CKPGUID_BOOL)
        {
            for (int i = 1; i < beh->GetInputParameterCount(); ++i)
            {
                pin = beh->GetInputParameter(i);
                if (pin->GetGUID() != CKPGUID_SCRIPT)
                {
                    pin->SetGUID(CKPGUID_SCRIPT);
                }
            }
        }
    }
    }

    return CKBR_OK;
}

CKERROR CreateActivateScriptProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Activate Script");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Reset ?", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Script", CKPGUID_SCRIPT);

    proto->DeclareSetting("Awake Object", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_VARIABLEPARAMETERINPUTS | CKBEHAVIOR_INTERNALLYCREATEDINPUTPARAMS));
    proto->SetFunction(ActivateScript);
    proto->SetBehaviorCallbackFct(ActivateScriptCallBack);

    *pproto = proto;
    return CK_OK;
}

int ActivateScript(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKParameterIn *pin;
    CKBehavior *script;

    CKBOOL k = TRUE;

    CKScene *scn = ctx->GetCurrentScene();

    pin = beh->GetInputParameter(0);
    if (pin->GetGUID() == CKPGUID_BOOL) // new version
    {
        //----- Get Boolean
        beh->GetInputParameterValue(0, &k);

        // Added by AGoTH due to the fix in the Scene::ActivateScript
        // the behavior in the 2.0 was to awake automatically the object
        CKBOOL awake = TRUE;
        beh->GetLocalParameterValue(0, &awake);

        for (int i = 1; i < beh->GetInputParameterCount(); ++i)
        {
            //----- Get Script
            script = (CKBehavior *)beh->GetInputParameterObject(i);
            if (!script)
                continue;

            scn->Activate(script, k);

            if (awake)
            {
                CKBeObject *beo = script->GetOwner();
                if (!beo->IsActiveInCurrentScene())
                {
                    behcontext.CurrentScene->Activate(beo, FALSE);
                }
            }
        }
    }
    else // old version
    {
        //----- Get Boolean
        beh->GetInputParameterValue(1, &k);

        //----- Get Script
        script = (CKBehavior *)beh->GetInputParameterObject(0);
        if (!script)
            return CKBR_PARAMETERERROR;

        scn->Activate(script, k);

        CKBeObject *beo = script->GetOwner();
        if (!beo->IsActiveInCurrentScene())
        {
            behcontext.CurrentScene->Activate(beo, FALSE);
        }
    }

    return CKBR_OK;
}
