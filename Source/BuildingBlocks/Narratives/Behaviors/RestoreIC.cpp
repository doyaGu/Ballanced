/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            RestoreIC
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorRestoreICDecl();
CKERROR CreateRestoreICProto(CKBehaviorPrototype **);
int RestoreIC(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorRestoreICDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Restore IC");
    od->SetDescription("Restores an object's Initial Condition for the scene being played.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x766e4e44, 0x4fac6d52));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRestoreICProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/States");
    return od;
}

CKERROR CreateRestoreICProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Restore IC");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RestoreIC);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RestoreIC(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    //----- Get Object
    CKBeObject *bo = (CKBeObject *)beh->GetTarget();
    if (!bo)
        return CKBR_OWNERERROR;

    CKScene *scn = behcontext.CurrentScene;
    if (!scn)
        return CKBR_GENERICERROR;
    //----- Restore the IC
    if (bo->IsInScene(scn))
    {
        CKStateChunk *chunk = scn->GetObjectInitialValue(bo);
        if (chunk)
        {
            CKReadObjectState(bo, chunk);
        }
    }

    return CKBR_OK;
}
