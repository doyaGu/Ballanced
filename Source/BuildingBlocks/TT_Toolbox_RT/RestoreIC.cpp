/////////////////////////////////
/////////////////////////////////
//
//        TT Restore IC
//
/////////////////////////////////
/////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorRestoreICDecl();
CKERROR CreateRestoreICProto(CKBehaviorPrototype **pproto);
int RestoreIC(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorRestoreICDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT Restore IC");
    od->SetDescription("Restores an object's Initial Condition for the scene being played.");
    od->SetCategory("TT Toolbox/Narratives");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x7e0f2f58, 0x232d3109));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateRestoreICProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateRestoreICProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT Restore IC");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Hierarchy?", CKPGUID_BOOL, "FALSE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(RestoreIC);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int RestoreIC(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}