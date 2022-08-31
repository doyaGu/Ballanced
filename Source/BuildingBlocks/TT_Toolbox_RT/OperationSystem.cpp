//////////////////////////////////////
//////////////////////////////////////
//
//        TT OperationSystem
//
//////////////////////////////////////
//////////////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTOperationSystemBehaviorProto(CKBehaviorPrototype **pproto);
int TTOperationSystem(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTOperationSystemDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT OperationSystem");
    od->SetDescription("reads operation system");
    od->SetCategory("TT Toolbox/System");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4c94621d,0x24fe2cf3));
    od->SetAuthorGuid(TERRATOOLS_GUID);
    od->SetAuthorName("Virtools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTOperationSystemBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTOperationSystemBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT OperationSystem");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("In");

    proto->DeclareOutput("Out");

    proto->DeclareOutParameter("PlattformId", CKPGUID_INT);
    proto->DeclareOutParameter("MinorVersion", CKPGUID_INT);
    proto->DeclareOutParameter("MajorVersion", CKPGUID_INT);

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTOperationSystem);

    *pproto = proto;
    return CK_OK;
}

int TTOperationSystem(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTOperationSystemCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}