/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            AttachScript
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorAttachScriptDecl();
CKERROR CreateAttachScriptProto(CKBehaviorPrototype **);
int AttachScript(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorAttachScriptDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Attach Script");
    od->SetDescription("Attach a script on an entity.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Dynamic?: </SPAN>must the script be attached permanently or not.<BR>
    <SPAN CLASS=pin>Script: </SPAN>script to be attached.<BR>
    <SPAN CLASS=pin>Activate?: </SPAN>must the script be activated.<BR>
    <SPAN CLASS=pin>Reset?: </SPAN>must the script be resetted.<BR>
    <BR>
    See Also: Deactivate Script.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x1dba5f4a, 0x14481212));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateAttachScriptProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Script Management");
    return od;
}

CKERROR CreateAttachScriptProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Attach Script");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Dynamic?", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Script", CKPGUID_SCRIPT);
    proto->DeclareInParameter("Activate?", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Reset?", CKPGUID_BOOL, "TRUE");

    proto->DeclareOutParameter("Created Script", CKPGUID_SCRIPT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);
    proto->SetFunction(AttachScript);

    *pproto = proto;
    return CK_OK;
}

int AttachScript(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    CKBeObject *beo = beh->GetTarget();
    if (!beo)
        return CKBR_OWNERERROR;

    CKBOOL dynamic = TRUE;
    beh->GetInputParameterValue(0, &dynamic);

    CKBehavior *script = (CKBehavior *)beh->GetInputParameterObject(1);
    if (!script)
        return CKBR_PARAMETERERROR;

    // We create a dependencies context
    CKDependenciesContext depcontext(ctx);
    if (dynamic)
        depcontext.SetCreationMode(CK_OBJECTCREATION_DYNAMIC);
    depcontext.StartDependencies(NULL);
    CK_ID id = script->GetID();
    depcontext.AddObjects(&id, 1);
    depcontext.Copy();

    CKBehavior *newscript = (CKBehavior *)depcontext.GetObjects(0);
    beh->SetOutputParameterObject(0, newscript);

    CKBeObject *oldbeo = newscript->GetOwner();
    if (oldbeo)
        oldbeo->RemoveScript(newscript->GetID());

    beo->AddScript(newscript);

    CKBOOL activate = TRUE;
    beh->GetInputParameterValue(2, &activate);

    CKBOOL reset = TRUE;
    beh->GetInputParameterValue(3, &reset);

    if (activate)
    {
        behcontext.CurrentScene->Activate(newscript, reset);

        if (!beo->IsActiveInCurrentScene())
        {
            behcontext.CurrentScene->Activate(beo, FALSE);
        }
    }

    return CKBR_OK;
}
