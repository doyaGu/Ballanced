/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ExecuteScript
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorExecuteScriptDecl();
CKERROR CreateExecuteScriptProto(CKBehaviorPrototype **);
int ExecuteScript(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorExecuteScriptDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Execute Script");
    od->SetDescription("Executes a script.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the activated script's process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Reset?: </SPAN>if TRUE, the script will be reset; if FALSE the script will be resumed from its previous state.<BR>
    <SPAN CLASS=pin>Script: </SPAN>script to be performed.<BR>
    <BR>
    */
    /* warning:
    The script will actually be performed in the next frame.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x706c5a40, 0x5bb31a0b));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateExecuteScriptProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Script Management");
    return od;
}

CKERROR CreateExecuteScriptProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Execute Script");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Reset ?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Script", CKPGUID_SCRIPT);

    proto->DeclareSetting("Awake Object", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ExecuteScript);

    *pproto = proto;
    return CK_OK;
}

int ExecuteScript(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    //----- Get Script
    CKBehavior *script = (CKBehavior *)beh->GetInputParameterObject(1);
    if (!script)
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        beh->ActivateOutput(0, FALSE);

        //----- Get Boolean
        CKBOOL k = FALSE;
        beh->GetInputParameterValue(0, &k);

        behcontext.CurrentScene->Activate(script, k);

        CKBOOL awake = TRUE;
        beh->GetLocalParameterValue(0, &awake);

        if (awake)
        {
            CKBeObject *beo = script->GetOwner();
            if (!beo->IsActiveInCurrentScene())
            {
                behcontext.CurrentScene->Activate(beo, FALSE);
            }
        }

        return CKBR_ACTIVATENEXTFRAME;
    }

    if (!script->IsActive())
    {
        beh->ActivateOutput(0);
        return CKBR_OK;
    }

    return CKBR_ACTIVATENEXTFRAME;
}
