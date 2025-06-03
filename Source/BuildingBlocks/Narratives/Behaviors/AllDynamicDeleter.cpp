/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
//		       Delete Dynamic Objects
//
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
#include "CKAll.h"

CKERROR CreateDynamicDeleterProto(CKBehaviorPrototype **pproto);
int DynamicDeleter(const CKBehaviorContext &context);
CKERROR DynamicDeleterCB(int ckm, CKBehavior *beh, void *arg);

extern void CleanUp(CKBehavior *beh);
extern void Load(CKBehavior *beh, CKScene *scn);

CKObjectDeclaration *FillDynamicDeleterDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("Delete Dynamic Objects");
    od->SetDescription("Deletes all the dynamic objects in memory");
    od->SetCategory("Narratives/Object Management");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0xcba3195, 0x53440e4c));
    od->SetAuthorGuid(VIRTOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateDynamicDeleterProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateDynamicDeleterProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("Delete Dynamic Objects");
    if (!proto)
        return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");
    proto->DeclareOutput("Out");

    proto->DeclareInParameter("Scene", CKPGUID_SCENE);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(DynamicDeleter);

    *pproto = proto;
    return CK_OK;
}

int DynamicDeleter(const CKBehaviorContext &context)
{
    CKBehavior *beh = context.Behavior;

    beh->ActivateInput(0, FALSE);
    beh->ActivateOutput(0);

    context.Context->DestroyAllDynamicObjects();

    return CKBR_OK;
}
