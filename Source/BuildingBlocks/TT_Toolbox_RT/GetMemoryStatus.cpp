//////////////////////////////////////
//////////////////////////////////////
//
//        TT GetMemoryStatus
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTGetMemoryStatusBehaviorProto(CKBehaviorPrototype **pproto);
int TTGetMemoryStatus(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTGetMemoryStatusDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT GetMemoryStatus");
    od->SetDescription("reads system memory status");
    od->SetCategory("TT Toolbox/System");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3b826e04,0x6e764285));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTGetMemoryStatusBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTGetMemoryStatusBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT GetMemoryStatus");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("MemoryLoad", CKPGUID_INT);
    proto->DeclareOutParameter("Total Physical Memory", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Available Physical Memory", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Total Virtual Address Space", CKPGUID_FLOAT);
    proto->DeclareOutParameter("Available Virtual Address Space", CKPGUID_FLOAT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTGetMemoryStatus);

    *pproto = proto;
    return CK_OK;
}

int TTGetMemoryStatus(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTGetMemoryStatusCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}