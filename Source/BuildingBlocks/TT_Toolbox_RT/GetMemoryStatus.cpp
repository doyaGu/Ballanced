//////////////////////////////////////
//////////////////////////////////////
//
//        TT GetMemoryStatus
//
//////////////////////////////////////
//////////////////////////////////////
#include "CKAll.h"
#include "ToolboxGuids.h"

CKObjectDeclaration *FillBehaviorGetMemoryStatusDecl();
CKERROR CreateGetMemoryStatusProto(CKBehaviorPrototype **pproto);
int GetMemoryStatus(const CKBehaviorContext &behcontext);

CKObjectDeclaration *FillBehaviorGetMemoryStatusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetMemoryStatus");
    od->SetDescription("Reads system memory status");
    od->SetCategory("TT Toolbox/System");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3b826e04, 0x6e764285));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateGetMemoryStatusProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateGetMemoryStatusProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetMemoryStatus");
    if (!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("MemoryLoad", CKPGUID_INT);
    proto->DeclareOutParameter("Total Physical Memory", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Available Physical Memory", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Total Virtual Address Space", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Available Virtual Address Space", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(GetMemoryStatus);

    *pproto = proto;
    return CK_OK;
}

int GetMemoryStatus(const CKBehaviorContext &behcontext)
{
    CKBehavior *beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}