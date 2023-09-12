///////////////////////////////////////
///////////////////////////////////////
//
//        TT ChildrenIterator
//
///////////////////////////////////////
///////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorChildrenIteratorDecl();
CKERROR CreateChildrenIteratorProto(CKBehaviorPrototype **pproto);
int ChildrenIterator(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorChildrenIteratorDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT ChildrenIterator");
    od->SetDescription("Retrieves each child of a hierarchy.");
    od->SetCategory("TT Toolbox/Logic");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x78041c90, 0x16de3cde));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateChildrenIteratorProto);
    od->SetCompatibleClassId(CKCID_3DENTITY);
    return od;
}

CKERROR CreateChildrenIteratorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT ChildrenIterator");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("Reset");
    proto->DeclareInput("Next");

    proto->DeclareOutput("Finished");
    proto->DeclareOutput("Not Finished");

    proto->DeclareOutParameter("Childrencount", CKPGUID_INT, "0");
    proto->DeclareOutParameter("Child", CKPGUID_BEOBJECT);
    proto->DeclareOutParameter("Index", CKPGUID_INT, "0");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(ChildrenIterator);

    proto->SetBehaviorFlags(CKBEHAVIOR_TARGETABLE);

    *pproto = proto;
    return CK_OK;
}

int ChildrenIterator(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;

    CK3dEntity *target = (CK3dEntity *)beh->GetTarget();
    if (!target)
        return CKBR_OWNERERROR;

    int index = 0;
    beh->GetOutputParameterValue(2, &index);

    int count = target->GetChildrenCount();
    if (beh->IsInputActive(0))
    {
        beh->ActivateInput(0, FALSE);
        index = 0;
        beh->SetOutputParameterValue(0, &count);
    }
    else
    {
        beh->ActivateInput(1, FALSE);
        ++index;
    }

    CKObject *child = NULL;
    if (index < count && (child = target->GetChild(index)) != NULL)
    {
        beh->SetOutputParameterObject(1, child);
        beh->SetOutputParameterValue(2, &index);
        beh->ActivateOutput(1, TRUE);

        return CKBR_OK;
    }

    beh->ActivateOutput(0, TRUE);
    return CKBR_OK;
}