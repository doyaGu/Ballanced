/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SaveState
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSaveStateDecl();
CKERROR CreateSaveStateProto(CKBehaviorPrototype **);
int SaveState(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorSaveStateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Save State");
    od->SetDescription("Saves an object state.");
    /* rem:
    <SPAN CLASS=in>In: </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out: </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pout>Object State: </SPAN>current state of the target, when the behavior is processed.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x200123d8, 0x65f802b6));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSaveStateProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/States");
    return od;
}

CKERROR CreateSaveStateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Save State");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("Object State", CKPGUID_STATECHUNK);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SaveState);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SaveState(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    //----- Get Object
    CKBeObject *bo = (CKBeObject *)beh->GetTarget();
    if (!bo)
        return CKBR_OWNERERROR;

    //----- Save the IC
    CKStateChunk *statechunk = CKSaveObjectState(bo);

    CKStateChunk *chunk;
    beh->GetOutputParameterValue(0, &chunk);

    chunk->Clone(statechunk);
    DeleteCKStateChunk(statechunk);

    CKParameterOut *pout = beh->GetOutputParameter(0);
    pout->DataChanged();

    return CKBR_OK;
}
