////////////////////////////
////////////////////////////
//
//        TT_Debug
//
////////////////////////////
////////////////////////////
#include "TT_Toolbox_RT.h"

CKERROR CreateTTDebugBehaviorProto(CKBehaviorPrototype **pproto);
int TTDebug(const CKBehaviorContext& behcontext);

CKObjectDeclaration *FillBehaviorTTDebugDecl()
{
    CKObjectDeclaration *od = CreateCKObjectDeclaration("TT_Debug");
    od->SetDescription("Debug ");
    od->SetCategory("TT Toolbox/Debug");
    od->SetType(CKDLL_BEHAVIORPROTOTYPE);
    od->SetGuid(CKGUID(0x4a446c43,0x66fa2375));
    od->SetAuthorGuid(CKGUID(0x53c80889,0x57f84916));
    od->SetAuthorName("Terratools");
    od->SetVersion(0x00010000);
    od->SetCreationFunction(CreateTTDebugBehaviorProto);
    od->SetCompatibleClassId(CKCID_BEOBJECT);
    return od;
}

CKERROR CreateTTDebugBehaviorProto(CKBehaviorPrototype **pproto)
{
    CKBehaviorPrototype *proto = CreateCKBehaviorPrototype("TT_Debug");
    if(!proto) return CKERR_OUTOFMEMORY;

    proto->DeclareInput("IN");

    proto->DeclareOutput("Exit");

    proto->DeclareInParameter("DebugText", CKPGUID_STRING);
    proto->DeclareInParameter("Console", CKPGUID_BOOL, "TRUE");
    proto->DeclareInParameter("Log-File", CKPGUID_BOOL, "FALSE");
    proto->DeclareInParameter("Enable", CKPGUID_BOOL, "TRUE");

    proto->SetFlags(CK_BEHAVIORPROTOTYPE_NORMAL);
    proto->SetFunction(TTDebug);

    *pproto = proto;
    return CK_OK;
}

int TTDebug(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}

CKERROR TTDebugCallBack(const CKBehaviorContext& behcontext)
{
    CKBehavior* beh = behcontext.Behavior;
    // TODO: To be finished.
    return CKBR_OK;
}