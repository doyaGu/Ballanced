/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		            SaveIC
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKObjectDeclaration *FillBehaviorSaveICDecl();
CKERROR CreateSaveICProto(CKBehaviorPrototype **);
int SaveIC(const CKBehaviorContext &context);

CKObjectDeclaration *FillBehaviorSaveICDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Set IC");
    od->SetDescription("Saves an object's Initial Condition in the scene being played.");
    /* rem:
    <SPAN CLASS=in>In : </SPAN>triggers the process.<BR>
    <SPAN CLASS=out>Out : </SPAN>is activated when the process is completed.<BR>
    <BR>
    */
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x72671a24, 0x161a2347));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateSaveICProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    od->SetCategory("Narratives/States");
    return od;
}

CKERROR CreateSaveICProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Set IC");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(SaveIC);
    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int SaveIC(const CKBehaviorContext &behcontext)
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

    //----- Save the IC
    if (bo->IsInScene(scn))
    {
        CKStateChunk *chunk = CKSaveObjectState(bo);
        scn->SetObjectInitialValue(bo, chunk);
    }

    return CKBR_OK;
}
