/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ActivateObject
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorActivateObjectDecl();
CKERROR CreateActivateObjectProto(CKBehaviorPrototype **);
int ActivateObject(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorActivateObjectDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Activate Object");
    od->SetDescription("Activates all the scripts of an object, either in the state they were before deactivation or once reset.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Object: </SPAN>object to be activated.<BR>
    <SPAN CLASS=pin>Reset?: </SPAN>if TRUE, all scripts will be reset; if FALSE the scripts will carry from the state they were in before being deactivated.<BR>
    <SPAN CLASS=pin>Activate All Scripts?: </SPAN>if TRUE, all scripts will be activated.<BR>
    <BR>
    See Also: Deactivate Object.<BR>
    */
    /* warning:
    The scripts will actually be re-activated in the next frame.
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x5f003fba, 0x5e574f60));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateActivateObjectProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/Script Management");
    return od;
}

CKERROR CreateActivateObjectProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Activate Object");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object", CKPGUID_BEOBJECT);
    proto->DeclareInParameter("Reset ?", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Activate All Scripts", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Reset Scripts", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ActivateObject);

    *pproto = proto;
    return CK_OK;
}

int ActivateObject(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    CKContext *ctx = behcontext.Context;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    //----- Get Object
    CKBeObject *bo = (CKBeObject *)beh->GetInputParameterObject(0);
    if (!bo)
        return CKBR_PARAMETERERROR;

    //----- Get Boolean
    CKBOOL k = FALSE;
    beh->GetInputParameterValue(1, &k);

    CKScene *scn = ctx->GetCurrentScene();
    if (scn)
        scn->Activate(bo, k);

    // Script Activation
    CKBOOL as = FALSE;
    beh->GetInputParameterValue(2, &as);

    if (as)
    {
        // Script Reset
        CKBOOL resetScript = k; // old functioning
        beh->GetInputParameterValue(3, &resetScript);

        for (int i = 0; i < bo->GetScriptCount(); ++i)
        {
            CKBehavior *script = bo->GetScript(i);
            if (scn)
                scn->Activate(script, resetScript);
        }
    }

    return CKBR_OK;
}
