///////////////////////////////////////
///////////////////////////////////////
//
//        TT ChildrenIterator
//
///////////////////////////////////////
///////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTChildrenIteratorBehaviorProto(CKBehaviorPrototype **pproto);
int TTChildrenIterator(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTChildrenIteratorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ChildrenIterator");
    od->SetDescription("Retrieves each child of a hierarchy.");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78041c90,0x16de3cde));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTChildrenIteratorBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTChildrenIteratorBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ChildrenIterator");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Reset");
    proto->DeclareInput("Next");

    proto->DeclareOutput("Finished");
    proto->DeclareOutput("Not Finished");

    proto->DeclareOutParameter("Childrencount", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Child", CKPGUID_BEOBJECT);
    proto->DeclareOutParameter("Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTChildrenIterator);

    proto->SetBehaviorFlags((CK_BEHAVIOR_FLAGS)(CKBEHAVIOR_TARGETABLE));
    *pproto = proto;
    return CK_OK;
}

int TTChildrenIterator(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTChildrenIteratorCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}