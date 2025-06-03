/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            ReadState
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorReadStateDecl();
CKERROR CreateReadStateProto(CKBehaviorPrototype **);
int ReadState(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorReadStateDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Read State");
    od->SetDescription("Read an object state.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    <SPAN CLASS=pin>Object State: </SPAN>state chunck containing a state of a compatible class id to read.<BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3ab6273f, 0x578f7118));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateReadStateProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/States");
    return od;
}

CKERROR CreateReadStateProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Read State");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Object State", CKPGUID_STATECHUNK);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ReadState);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int ReadState(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    //----- Get Object
    CKBeObject *bo = (CKBeObject *)beh->GetTarget();
    if (!bo)
        return CKBR_OWNERERROR;

    CKStateChunk *chunk;
    beh->GetInputParameterValue(0, &chunk);

    CKReadObjectState(bo, chunk);

    return CKBR_OK;
}
