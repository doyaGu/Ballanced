//////////////////////////////
//////////////////////////////
//
//        TT_DebugON
//
//////////////////////////////
//////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTDebugONBehaviorProto(CKBehaviorPrototype **pproto);
int TTDebugON(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTDebugONDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_DebugON");
    od->SetDescription("Debug ON");
    od->SetCategory("TT Toolbox/Debug");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x3d00718f,0x2c8b33a2));
    od->SetAuthorGuid(CKGUID(0x53c80889,0x57f84916));
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTDebugONBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTDebugONBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_DebugON");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Exit");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTDebugON);

    *pproto = proto;
    return CK_OK;
}

int TTDebugON(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTDebugONCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}