///////////////////////////////
///////////////////////////////
//
//        TT_DebugOFF
//
///////////////////////////////
///////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTDebugOFFBehaviorProto(CKBehaviorPrototype **pproto);
int TTDebugOFF(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTDebugOFFDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_DebugOFF");
    od->SetDescription("Debug OFF");
    od->SetCategory("TT Toolbox/Debug");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x70f65b3e,0x2dc41372));
    od->SetAuthorGuid(CKGUID(0x53c80889,0x57f84916));
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTDebugOFFBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTDebugOFFBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_DebugOFF");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Exit");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTDebugOFF);

    *pproto = proto;
    return CK_OK;
}

int TTDebugOFF(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTDebugOFFCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}